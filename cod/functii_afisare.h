#ifndef FUNCTII_AFISARE_H_
#define FUNCTII_AFISARE_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  SEND_SUCCESSFUL,
  SEND_NOT_SUCCESSFUL
} send_status_t;

send_status_t setup_7_segm(void);
send_status_t afisare_tensiune(uint16_t mV);

void Display_Task_Run(uint16_t valoare_tensiune);

bool Display_Is_Error(void);

#endif