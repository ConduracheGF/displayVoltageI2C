#include "functii_afisare.h"
#include "functii_i2c.h"
#include "functii_timer.h"

typedef enum {
  DRIVER_NEEDS_INIT,
  DRIVER_OK
} driver_state_t;

static driver_state_t driver_state = DRIVER_NEEDS_INIT;

static send_status_t send_i2c(uint8_t addr_dummy, uint8_t reg, uint8_t val) {
    uint8_t data[2];
    data[0] = reg;
    data[1] = val;
    uint8_t i2c_addr = 0x00; 

    while(i2c_is_busy());
    if(i2c_write_transaction(i2c_addr, data, 2) != I2C_STATUS_SUCCESS) return SEND_NOT_SUCCESSFUL;
    while(i2c_is_busy());
    
    if(i2c_get_last_status() == I2C_STATUS_SUCCESS) return SEND_SUCCESSFUL;
    return SEND_NOT_SUCCESSFUL;
}

send_status_t setup_7_segm(){
  if(send_i2c(0x00, 0x09, 0xFF) != SEND_SUCCESSFUL) return SEND_NOT_SUCCESSFUL;
  if(send_i2c(0x00, 0x0A, 0x0F) != SEND_SUCCESSFUL) return SEND_NOT_SUCCESSFUL;
  if(send_i2c(0x00, 0x0B, 0x03) != SEND_SUCCESSFUL) return SEND_NOT_SUCCESSFUL;
  if(send_i2c(0x00, 0x0C, 0x01) != SEND_SUCCESSFUL) return SEND_NOT_SUCCESSFUL;
  return SEND_SUCCESSFUL;
}

static send_status_t afisare(int cifra[]) {
    for (int i = 0; i < 4; i++) {
      if(send_i2c(0x00, i + 1, cifra[3-i]) != SEND_SUCCESSFUL) return SEND_NOT_SUCCESSFUL;
    }
    return SEND_SUCCESSFUL;
}

send_status_t afisare_tensiune(uint16_t mV) {
  int cifra[4] = {0};
  if (mV > 5000) mV = 5000;
  cifra[0] = ((mV/1000)%10) | 0x80;
  cifra[1] = (mV/100)%10;
  cifra[2] = (mV/10)%10;
  cifra[3] = mV%10;
  return afisare(cifra);
}

void Display_Task_Run(uint16_t valoare_tensiune) {
    static uint16_t safety_counter = 0;
    
    switch (driver_state) {
        case DRIVER_NEEDS_INIT:
            if (setup_7_segm() == SEND_SUCCESSFUL) {
                driver_state = DRIVER_OK;
                safety_counter = 0;
            }
            break;
            
        case DRIVER_OK:
            if (afisare_tensiune(valoare_tensiune) != SEND_SUCCESSFUL) {
                driver_state = DRIVER_NEEDS_INIT;
            }
            // Mecanism simplu de safety (re-init periodic)
            if (++safety_counter >= 100) driver_state = DRIVER_NEEDS_INIT;
            break;
    }
}

bool Display_Is_Error(void) {
    return (driver_state == DRIVER_NEEDS_INIT);
}