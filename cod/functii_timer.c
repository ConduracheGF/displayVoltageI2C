#include "functii_timer.h"
#include <ioavr.h>
#include "functii_timer.h"
#include <ioavr.h>
#include <intrinsics.h>

volatile uint16_t ms_ticks = 0;
static timer_state_t timer_state = TIMER_COUNTER;
static uint16_t start_time = 0;

void Timer0_Init(void) {
    TCCR0A = (1 << WGM01); // CTC
    TCCR0B = (1 << CS01) | (1 << CS00); // Prescaler 64
    OCR0A = 249; // 1ms la 16MHz
    TIMSK0 |= (1 << OCIE0A); 
}

#pragma vector = TIMER0_COMPA_vect
__interrupt void TIMER0_ISR(void) {
    ms_ticks++;
}

// Aceasta functie gestioneaza masina de stari a timer-ului
bool Timer_Check(void) {
    bool ready = false;
    
    switch (timer_state) {
        case TIMER_COUNTER:
            start_time = ms_ticks;
            timer_state = TIMER_WAIT;
            break;

        case TIMER_WAIT:
            if ((uint16_t)(ms_ticks - start_time) >= 200) {
                timer_state = TIMER_EXPIRED;
            }
            break;

        case TIMER_EXPIRED:
            ready = true;
            timer_state = TIMER_COUNTER; // Resetam automat pentru urmatorul ciclu
            break;
    }
    return ready;
}

void delay_secunde(uint16_t time){
    volatile uint32_t i;
    while (time--) {
        i = 0;
        while (i < 2000000UL) i++;
    }
}

void delay_milisecunde(uint32_t time){
    volatile uint32_t i;
    while (time--) {
        i = 0;
        while (i < 2000UL) i++;
    }
}

void delay_microsecunde(uint16_t time){
    volatile uint16_t i;
    while (time--) {
        i = 0;
        while (i < 2) i++;
    }
}