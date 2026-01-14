#include "functii_adc.h"
#include <avr/io.h>
#include <stdint.h>

void ADC_Init(void){
    ADMUX = (1 << REFS0); //avcc 5V
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //prescaler 128
}

uint16_t ADC_Read(void){
    ADCSRA |= (1 << ADSC); //start conversie
    while (ADCSRA & (1 << ADSC)); //asteapta finalizarea si dupa incepe
    uint16_t val = ADCL;
    val |= (ADCH  << 8);
    return val;
}