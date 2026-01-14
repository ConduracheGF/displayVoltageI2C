#include "functii_i2c.h"
#include "functii_uart.h"
#include <avr/io.h>

void setup_i2c(){
    TWSR = 0x00;
    TWBR = 72;
    TWCR = (1<<TWEN);
}

static uint8_t wait_twsto(){
    uint32_t timeout = TIMEOUT;
    while (TWCR & (1<<TWSTO)){
        if (--timeout == 0){
            return 0;
        }
    }
    return 1;
}

static uint8_t wait_twint(){
    uint32_t timeout = TIMEOUT;
    while(!(TWCR & (1 << TWINT))){
        if (--timeout == 0){
            return 0;
        }
    }
    return 1;
}

static void recover_bus_i2c(){
    //dezactivare twi
    TWCR &= ~(1<<TWEN);

    //sda e pd1, scl e pd0
    DDRD |= (1<<PD0);
    DDRD &= ~(1<<PD1);

    //pull up sda
    PORTD |= (1<<PD1);

    for(uint8_t i=0; i<10; i++){
        PORTD &= ~(1<<PD0);
        PORTD |= (1<<PD0);
    }

    setup_i2c();
}

send_status_t send_i2c(unsigned int address, unsigned int reg, unsigned int data){
    if (i2c_bus_state == I2C_STUCK){
        recover_bus_i2c();
        i2c_bus_state = I2C_OK;
    }

    //start
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    if(!wait_twint() || (TWSR & 0xf8) != START){
        i2c_bus_state = I2C_STUCK;
        return SEND_NOT_SUCCESSFUL;
    }

    //adresa
    TWDR = address << 1;
    TWCR = (1<<TWINT) | (1<<TWEN);
    if(!wait_twint() || (TWSR & 0xf8) != MT_SLA_ACK){
        i2c_bus_state = I2C_STUCK;
        return SEND_NOT_SUCCESSFUL;
    }

    //reg
    TWDR = reg;
    TWCR = (1<<TWINT) | (1<<TWEN);
    if(!wait_twint() || (TWSR & 0xf8) != MT_DATA_ACK){
        i2c_bus_state = I2C_STUCK;
        return SEND_NOT_SUCCESSFUL;
    }

    //data
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    if(!wait_twint() || (TWSR & 0xf8) != MT_DATA_ACK){
        i2c_bus_state = I2C_STUCK;
        return SEND_NOT_SUCCESSFUL;
    }

    //stop
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
    if(!wait_twsto()){
        i2c_bus_state = I2C_STUCK;
        return SEND_NOT_SUCCESSFUL;
    }

    i2c_bus_state = I2C_OK;
    return SEND_SUCCESSFUL;
}