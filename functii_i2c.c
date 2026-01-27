#include "functii_i2c.h"
#include <ioavr.h>
#include <inavr.h>
#include <stdint.h>


//starea curenta
volatile i2c_state_t g_i2c_state = I2C_STATE_IDLE;
//ultimul status returnat
static volatile i2c_status_t g_i2c_status = I2C_STATUS_SUCCESS;
static uint8_t init_step = 0;

/*
functia initializeaza modulul TWI/I2C.
configureaza prescalerul si frecventa de lucru (SCL_CLOCK).
activeaza perifericul TWI (fara a activa întreruperile inca).
*/
void setup_i2c(void) {
  TWSR = 0x00; //prescaler = 1
  //baud-rate generator pentru SCL
  TWBR = 72; //100kHz
  //activeaza TWI, fara întreruperi
  TWCR = (1 << TWEN);
  g_i2c_state = I2C_STATE_IDLE;
}

static uint8_t wait_twint() {
    uint32_t timeout = TIMEOUT;
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) return 0;
    }
    return 1;
}

static uint8_t wait_twsto() {
    uint32_t timeout = TIMEOUT;
    while (TWCR & (1 << TWSTO)) {
        if (--timeout == 0) return 0;
    }
    return 1;
}

static void recover_bus_i2c(){
    uint32_t timeout = TIMEOUT/20;
  
    //dezactivare twi
    TWCR &= ~(1<<TWEN);

    //sda e pd1, scl e pd0
    DDRD |= (1<<PD0);
    DDRD &= ~(1<<PD1);

    //pull up sda
    PORTD |= (1<<PD1);

    for(uint8_t i=0; i<10; i++){
      while(timeout){timeout--;}
      PORTD &= ~(1<<PD0);
      while(timeout){timeout--;}
      PORTD |= (1<<PD0);
      while(timeout){timeout--;}
      if (PIND & (1 << PD1)) break;
    }

    setup_i2c();
}

// TRANSMITERE DATE

i2c_status_t send_i2c(unsigned int address, unsigned int reg, unsigned int data){
    //start
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    if(!wait_twint() || (TWSR & 0xf8) != TW_START){
        g_i2c_state = I2C_STATE_STUCK;
        return I2C_STATUS_ERROR;
    }

    //adresa
    TWDR = address << 1;
    TWCR = (1<<TWINT) | (1<<TWEN);
    if(!wait_twint() || (TWSR & 0xf8) != TW_MT_SLA_ACK){
        g_i2c_state = I2C_STATE_STUCK;
        return I2C_STATUS_ERROR;
    }

    //reg
    TWDR = reg;
    TWCR = (1<<TWINT) | (1<<TWEN);
    if(!wait_twint() || (TWSR & 0xf8) != TW_MT_DATA_ACK){
        g_i2c_state = I2C_STATE_STUCK;
        return I2C_STATUS_ERROR;
    }

    //data
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    if(!wait_twint() || (TWSR & 0xf8) != TW_MT_DATA_ACK){
        g_i2c_state = I2C_STATE_STUCK;
        return I2C_STATUS_ERROR;
    }

    //stop
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
    if(!wait_twsto()){
        g_i2c_state = I2C_STATE_STUCK;
        return I2C_STATUS_ERROR;
    }

    g_i2c_state = I2C_STATE_IDLE;
    return I2C_STATUS_SUCCESS;
}

//LOGICA DE AFISARE PE SEGMENTE

i2c_status_t setup_7_segm(){
  uint8_t reg, data;
  switch (init_step) {
  case 0: reg = 0x09; data = 0xFF; break; //decode mode
  case 1: reg = 0x0A; data = 0x0F; break; //intensity
  case 2: reg = 0x0B; data = 0x03; break; //scan limit
  case 3: reg = 0x0C; data = 0x01; break; //shutdown -> normal
  default: return I2C_STATUS_SUCCESS;
  }
  
  if (send_i2c(TW_WRITE_ADDRESS, reg, data) == I2C_STATUS_SUCCESS) {
    init_step++;
    if (init_step > 3) {
      init_step = 0;
      return I2C_STATUS_SUCCESS;
    }
    return I2C_STATUS_BUSY;
  }
  return I2C_STATUS_ERROR;
}

static i2c_status_t afisare(int cifra[]) {
  for (int i = 0; i < 4; i++) {
    if(send_i2c(TW_WRITE_ADDRESS, i + 1, cifra[3-i]) != I2C_STATUS_SUCCESS) 
      return I2C_STATUS_ERROR;
  }
  return I2C_STATUS_SUCCESS;
}

i2c_status_t afisare_tensiune(uint16_t mV) {
  int cifra[4] = {0};
  if (mV > 5000) mV = 5000;
  cifra[0] = ((mV/1000)%10) | 0x80;
  cifra[1] = (mV/100)%10;
  cifra[2] = (mV/10)%10;
  cifra[3] = mV%10;
  return afisare(cifra);
}

//AUTOMATUL DE STARI I2C

i2c_status_t I2C_Task_Run(uint16_t valoare_tensiune, uint8_t force_init){
    //daca driverul cere init manual sau safety counter a expirat in driver
    if (force_init) {
        init_step = 0;
        g_i2c_state = I2C_STATE_IDLE; //resetam starea pentru a reincepe
    }

    switch (g_i2c_state) {
        case I2C_STATE_STUCK:
            recover_bus_i2c();
            init_step = 0;
            g_i2c_state = I2C_STATE_IDLE;
            return I2C_STATUS_ERROR; //raportam ca am fost blocati

        case I2C_STATE_IDLE:
            //incercam afisarea
            if (afisare_tensiune(valoare_tensiune) == I2C_STATUS_SUCCESS) {
                return I2C_STATUS_SUCCESS;
            } else {
                //afisare_tensiune a setat deja g_i2c_state pe STUCK daca a dat eroare
                return I2C_STATUS_ERROR;
            }

        case I2C_STATE_WRITING:
            //putem folosi asta daca implementam setup_7_segm pas cu pas aici
            if (setup_7_segm() == I2C_STATUS_SUCCESS) {
                g_i2c_state = I2C_STATE_IDLE;
                return I2C_STATUS_SUCCESS;
            }
            return I2C_STATUS_BUSY;

        default:
            return I2C_STATUS_ERROR;
    }
}