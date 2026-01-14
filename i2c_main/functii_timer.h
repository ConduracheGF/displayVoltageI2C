#ifndef FUNCTII_TIMER_H_
#define FUNCTII_TIMER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void delay_secunde(uint16_t time);
void delay_milisecunde(uint32_t time);
void delay_microsecunde(uint16_t time);

#ifdef __cplusplus
}
#endif

#endif