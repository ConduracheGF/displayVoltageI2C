#ifndef FUNCTII_AFISARE_H_
#define FUNCTII_AFISARE_H_

#include <stdint.h>
#include "state_machine.h"

#ifdef __cplusplus
extern "C" {
#endif

send_status_t setup_7_segm();
send_status_t afisare_tensiune(uint16_t mV);

#ifdef __cplusplus
}
#endif

#endif