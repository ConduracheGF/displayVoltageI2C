#ifndef FUNCTII_I2C_H_
#define FUNCTII_I2C_H_

#include <stdint.h>
#include "state_machine.h"

#ifdef __cplusplus
extern "C" {
#endif

#define START 0x08
#define MT_SLA_ACK 0x18
#define MT_DATA_ACK 0x28
#define TIMEOUT 50000

void setup_i2c();
send_status_t send_i2c(unsigned int address, unsigned int reg, unsigned int data);

#ifdef __cplusplus
}
#endif

#endif