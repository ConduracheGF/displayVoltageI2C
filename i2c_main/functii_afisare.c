#include "functii_afisare.h"
#include "functii_i2c.h"
#include "functii_timer.h"

send_status_t setup_7_segm(){
  if(send_i2c(0x00, 0x09, 0xFF) != SEND_SUCCESSFUL){
    return SEND_NOT_SUCCESSFUL;
  }
  //delay_milisecunde(5);
  
  if(send_i2c(0x00, 0x0A, 0x0F) != SEND_SUCCESSFUL){
    return SEND_NOT_SUCCESSFUL;
  }
  //delay_milisecunde(5);
  
  if(send_i2c(0x00, 0x0B, 0x03) != SEND_SUCCESSFUL){
    return SEND_NOT_SUCCESSFUL;
  }
  //delay_milisecunde(5);
  
  if(send_i2c(0x00, 0x0C, 0x01) != SEND_SUCCESSFUL){
    return SEND_NOT_SUCCESSFUL;
  }
  //delay_milisecunde(5);
  
  return SEND_SUCCESSFUL;
}

static send_status_t afisare(int cifra[]) {
    // trimite cifrele catre AS1115
    for (int i = 0; i < 4; i++) {
      if(send_i2c(0x00, i + 1, cifra[3-i]) != SEND_SUCCESSFUL){
        return SEND_NOT_SUCCESSFUL;
      }
      //delay_milisecunde(2);
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