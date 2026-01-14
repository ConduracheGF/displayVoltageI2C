#include "functii_timer.h"

void delay_secunde(uint16_t time){
    volatile uint32_t i;

    while (time--) {
        i = 0;
        while (i < 2000000UL) {
            i++;
        }
    }
}

void delay_milisecunde(uint32_t time){
    volatile uint32_t i;

    while (time--) {
        i = 0;
        while (i < 2000UL) {
            i++;
        }
    }
}

void delay_microsecunde(uint16_t time){
    volatile uint16_t i;

    while (time--) {
        i = 0;
        while (i < 2) {
            i++;
        }
    }
}