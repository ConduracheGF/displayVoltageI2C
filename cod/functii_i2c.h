#ifndef FUNCTII_I2C_H
#define FUNCTII_I2C_H

#include <stdint.h>

#ifndef TIMEOUT
#define TIMEOUT 10000UL
#endif

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Conditie START transmisa
#define TW_START 0x08
// Master Transmit: adresa + W transmisa, ACK primit
#define TW_MT_SLA_ACK 0x18
// Master Transmit: octet de date transmis, ACK primit
#define TW_MT_DATA_ACK 0x28
// Adresa de transmisie
#define TW_WRITE_ADDRESS 0x00

/*
Statusul tranzactiei I2C
Enumerarea codurilor de stare pentru tranzactiile I2C.
Utila pentru depanare si gestionarea non-blocanta a transferurilor.
*/
typedef enum {
  I2C_STATUS_SUCCESS, //tranzactia a fost pornita cu succes
  I2C_STATUS_BUSY, //magistrala este ocupata cu o alta tranzac?ie
  I2C_STATUS_ERROR //eroare pe magistrala (NACK primit)
} i2c_status_t;

/*
Reprezinta starea curenta a modulului I2C/TWI.
Utilizata pentru a coordona transferurile non-blocante.
*/
typedef enum {
  I2C_STATE_IDLE, //nicio tranzactie în desfasurare
  I2C_STATE_WRITING, //se efectueaza o tranzactie de scriere
  I2C_STATE_STUCK //se efectueaza un moment de busire
} i2c_state_t;

i2c_status_t setup_7_segm(void);
i2c_status_t afisare_tensiune(uint16_t mV);
i2c_status_t send_i2c(unsigned int address, unsigned int reg, unsigned int val);
i2c_status_t I2C_Task_Run(uint16_t valoare_tensiune, uint8_t force_init);
void setup_i2c(void);

#endif