#include "functii_i2c.h"
#include <ioavr.h>
#include <inavr.h>
#include <stdint.h>

// Conditie START transmisa
#define TW_START 0x08
// Master Transmit: adresa + W transmisa, ACK primit
#define TW_MT_SLA_ACK 0x18
// Master Transmit: octet de date transmis, ACK primit
#define TW_MT_DATA_ACK 0x28
// Master Receive: adresa + R transmisa, ACK primit
#define TW_MR_SLA_ACK 0x40
// Master Receive: octet de date recep?ionat, ACK returnat
#define TW_MR_DATA_ACK 0x50
// Master Receive: octet de date recep?ionat, NACK returnat
#define TW_MR_DATA_NACK 0x58 


// Starea curenta
volatile i2c_state_t g_i2c_state = I2C_STATE_IDLE;
// Ultimul status returnat
static volatile i2c_status_t g_i2c_status = I2C_STATUS_SUCCESS;
static uint8_t g_slave_address; // Adresa slave (cu bitul R/W inclus)
static uint8_t* g_data_ptr; // Pointer catre bufferul de date
static uint8_t g_data_len; // Lungimea totala a transferului
static uint8_t g_data_idx; // Indexul curent în buffer

/*
* Functia initializeaza modulul TWI/I2C.
* - Configureaza prescaler-ul si frecven?a de lucru (SCL_CLOCK).
* - Activeaza perifericul TWI (fara a activa întreruperile înca).
*/
void i2c_init(void) {
  TWSR = 0x00; // Prescaler = 1
  // Baud-rate generator pentru SCL
  TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2;
  // Activeaza TWI, fara întreruperi
  TWCR = (1 << TWEN);
}

/*
* Func?ia verifica daca magistrala I2C este ocupata.
* Returneaza:
* - 1 daca o tranzac?ie este în desfa?urare
* - 0 daca modulul este inactiv (IDLE)
*/
uint8_t i2c_is_busy(void) {
  return (g_i2c_state != I2C_STATE_IDLE);
}


/*
* Functia returneaza ultimul status înregistrat al tranzac?iei.
* Poate fi folosita pentru depanare.
*/
i2c_status_t i2c_get_last_status(void) {
  return g_i2c_status;
}


/*
* Func?ia porne?te o tranzactie de scriere NON-BLOCANTA.
* Returneaza:
* - I2C_STATUS_SUCCESS daca tranzactia a fost pornita
* - I2C_STATUS_BUSY daca magistrala era deja ocupata
*/
i2c_status_t i2c_write_transaction(uint8_t address, uint8_t* data, uint8_t length) {
  if (g_i2c_state != I2C_STATE_IDLE) {
    return I2C_STATUS_BUSY;
  }
  g_i2c_state = I2C_STATE_WRITING;
  g_i2c_status = I2C_STATUS_BUSY;
  g_slave_address = (address << 1) | TW_WRITE; // Adresa + bit W
  g_data_ptr = data;
  g_data_len = length;
  g_data_idx = 0;
  // Lansam tranzactia prin generarea condi?iei START + întrerupere
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
  
  return I2C_STATUS_SUCCESS;
}

/*
* Func?ia porne?te o tranzac?ie de citire NON-BLOCANTA.
* Returneaza:
* - I2C_STATUS_SUCCESS daca tranzac?ia a fost pornita
* - I2C_STATUS_BUSY daca magistrala era deja ocupata
*/
i2c_status_t i2c_read_transaction(uint8_t address, uint8_t* data, uint8_t length) {
  if (g_i2c_state != I2C_STATE_IDLE) {
    return I2C_STATUS_BUSY; // Magistrala e ocupata
  }
  g_i2c_state = I2C_STATE_READING;
  g_i2c_status = I2C_STATUS_BUSY;
  g_slave_address = (address << 1) | 1; // Adresa + bit R
  g_data_ptr = data;
  g_data_len = length;
  g_data_idx = 0;
  // Pentru citire, ini?iem cu un START
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
  return I2C_STATUS_SUCCESS;
}

/*
* Aceasta rutina implementeaza logica ma?inii de stari pentru TWI/I2C.
* Gestioneaza atât opera?iile de scriere, cât ?i pe cele de citire,
* pe baza valorii registrului TWSR (status).
*/
#pragma vector = TWI_vect
__interrupt void TWI_ISR(void) {
  switch (TWSR & 0xF8) { // Se mascheaza doar bi?ii de status
  case TW_START: // Condi?ie START trimisa
    TWDR = g_slave_address; // Se trimite adresa + bit R/W
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
    break;
    // Slave a raspuns cu ACK dupa adresa
  case TW_MT_SLA_ACK:
    // Slave a raspuns cu ACK dupa un octet de date
  case TW_MT_DATA_ACK:
    if (g_data_idx < g_data_len) {
      // Mai sunt date de transmis
      TWDR = g_data_ptr[g_data_idx++];
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
    } else {
      // S-a terminat transmisia
      TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
      g_i2c_status = I2C_STATUS_SUCCESS;
      g_i2c_state = I2C_STATE_IDLE;
    }
    break;
    // S-a trimis adresa + R, Slave a raspuns cu ACK
  case TW_MR_SLA_ACK:
    // Daca mai este mai mult de 1 octet de citit e returnat ACK
    if (g_data_len - g_data_idx > 1) {
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
    } else {
      // Ultimul octet, deci se returneaza NACK
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
    }
    break;
  case TW_MR_DATA_ACK: // S-a citit un octet ?i s-a trimis ACK
    g_data_ptr[g_data_idx++] = TWDR;
    if (g_data_len - g_data_idx > 1) {
      // Mai sunt date de citit, deci se cere cu ACK
      TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWIE) | (1 << TWEA);
    } else {
      // Urmeaza ultimul octet, deci se cere fara ACK
      TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
    }
    break;
    // S-a citit ultimul octet ?i s-a trimis NACK
  case TW_MR_DATA_NACK:
    g_data_ptr[g_data_idx++] = TWDR;
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // STOP
    g_i2c_status = I2C_STATUS_SUCCESS;
    g_i2c_state = I2C_STATE_IDLE;
    break;
  default:
    g_i2c_status = I2C_STATUS_ERROR; // Se semnaleaza eroarea
    // STOP – se elibereaza magistrala
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    g_i2c_state = I2C_STATE_IDLE;
    break;
  }
}
