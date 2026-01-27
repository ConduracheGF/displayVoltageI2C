#include "adc.h"
#include <ioavr.h>
#include <intrinsics.h>

//variabile de stare si date
static volatile adc_state_t adc_state = ADC_REPAUS;
static volatile uint16_t tensiune_raw = 0;
static volatile bool data_ready_flag = false;

void ADC_Init(void) {
    ADMUX = (1 << REFS0); //referinta AVCC pe 3.3V
    
    //configuram prescaler 128
    ADCSRA = (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    adc_state = ADC_REPAUS;
}

#pragma vector = ADC_vect
__interrupt void ADC_ISR(void) {
    //achizitia de date
    tensiune_raw = ADC; 

    //oprirea ADCului
    ADCSRA &= ~(1 << ADEN);

    adc_state = ADC_READY;
}

void ADC_Task_Run(void) {
    switch (adc_state) {
        case ADC_REPAUS:
            //daca am consumat datele anterioare, pornim o noua sesiune
            if (!data_ready_flag) {
                ADCSRA |= (1 << ADEN); //activare bit alimentare ADC
                ADCSRA |= (1 << ADSC); //start conversie
                adc_state = ADC_CONVERT;
            }
            break;

        case ADC_CONVERT:
            //aici nu facem nimic, asteptam intreruperea sa actualizeze starea)
            break;

        case ADC_READY:
            //datele sunt în tensiune_raw, gata de procesat/filtrat
            data_ready_flag = true; 
            
            //ne întoarcem în repaus (ADCul e oprit din intreruperi)
            adc_state = ADC_REPAUS;
            break;
    }
}

//verifica in main daca avem date noi
bool ADC_New_Data_Available(void) {
    if (data_ready_flag) {
        data_ready_flag = false;
        return true;
    }
    return false;
}

//ne da ultima valoare a adcului
uint16_t ADC_Get_Last_Value(void) {
    return tensiune_raw;
}