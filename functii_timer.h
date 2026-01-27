#ifndef FUNCTII_TIMER_H_
#define FUNCTII_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TIMER_COUNTER,
    TIMER_WAIT,
    TIMER_EXPIRED
} timer_state_t;

//variabila globala incrementata in ISR
extern volatile uint16_t ms_ticks;

void Timer0_Init(void);
//functie care returneaza true o data la 20ms
bool Timer_Check(void);

//functii delay nefolosite (erau in proiectul vechi)
void delay_secunde(uint16_t time);
void delay_milisecunde(uint32_t time);
void delay_microsecunde(uint16_t time);

#endif