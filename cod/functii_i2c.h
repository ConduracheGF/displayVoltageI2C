#ifndef FUNCTII_I2C_H
#define FUNCTII_I2C_H

#include <stdint.h>
#include <inavr.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
// Delay specializat
#define DELAY_MS(ms) __delay_cycles((F_CPU / 1000UL) * (ms))

// Frecven?a ceasului I2C (100 kHz)
#define SCL_CLOCK 100000UL
// Direc?ia tranzac?iei: scriere catre Slave
#define TW_WRITE 0

/*
* Status-ul tranzac?iei I2C
* Enumerarea codurilor de stare pentru tranzac?iile I2C.
* Utila pentru depanare ?i gestionarea non-blocanta a transferurilor.
*/
typedef enum {
  I2C_STATUS_SUCCESS, // Tranzac?ia a fost pornita cu succes
  I2C_STATUS_BUSY, // Magistrala este ocupata cu o alta tranzac?ie
  I2C_STATUS_ERROR // Eroare pe magistrala (ex: NACK primit)
} i2c_status_t;

/*
* Reprezinta starea curenta a modulului I2C/TWI.
* Utilizata pentru a coordona transferurile non-blocante.
*/
typedef enum {
  I2C_STATE_IDLE, // Nicio tranzactie în desfasurare
  I2C_STATE_WRITING, // Se efectueaza o tranzactie de scriere
  I2C_STATE_READING // Se efectueaza o tranzactie de citire
} i2c_state_t;

void i2c_init(void);
/*
* Functia porne?te o tranzac?ie de scriere non-blocanta catre un
* dispozitiv Slave.
*/
i2c_status_t i2c_write_transaction(uint8_t address, uint8_t* data,
                                   uint8_t length);
// Functia verifica daca o tranzac?ie este în desfa?urare.
uint8_t i2c_is_busy(void);
/*
* Functia returneaza ultimul status al tranzac?iei I2C.
* Util pentru diagnostic ?i depanare.
*/
i2c_status_t i2c_get_last_status(void);
#endif