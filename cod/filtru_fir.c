#include "filtru_fir.h"

static uint16_t samples[FILTER_TAP_NUM];
static uint8_t index = 0;
static uint32_t sum = 0;

void FIR_Init(void) {
    for (uint8_t i = 0; i < FILTER_TAP_NUM; i++) {
        samples[i] = 0;
    }
    sum = 0;
    index = 0;
}

uint16_t FIR_Update(uint16_t new_sample) {
    //scadem esantionul cel mai vechi din suma totala
    sum -= samples[index];
    
    //adaugam noul esantion
    samples[index] = new_sample;
    sum += samples[index];
    
    //incrementam indexul circular
    index = (index + 1) & FILTER_MASK;
    
    return (uint16_t)(sum >> FILTER_SHIFT); 
}