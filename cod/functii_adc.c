#include "functii_adc.h"
#include <ioavr.h>

typedef enum {
  ADC_REPAUS,
  ADC_CONVERT,
  ADC_READY
} adc_state_t;

static adc_state_t adc_state = ADC_REPAUS;
static uint16_t tensiune_raw = 0;
static uint16_t tensiune_filtrata = 0;
static bool data_ready_flag = false;

void ADC_Init(void){
  ADMUX = (1 << REFS0);   
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_Get_Result(void){
    uint16_t val = ADCL;
    val |= (ADCH << 8);
    return val;
}

// Functie interna pentru filtrare (FIR)
static void fir_filter_process(uint16_t raw_val) {
    static uint32_t buffer_fir[8] = {0};
    static uint8_t idx = 0;
    
    buffer_fir[idx] = raw_val;
    idx = (idx + 1) % 8;
    
    uint32_t suma = 0;
    for(uint8_t i=0; i<8; i++) suma += buffer_fir[i];
    
    tensiune_filtrata = suma / 8;
    data_ready_flag = true;
}

void ADC_Task_Run(void) {
    switch (adc_state) {
        case ADC_REPAUS:
            // Pornim o noua conversie doar daca flag-ul a fost consumat
            if (!data_ready_flag) {
                ADCSRA |= (1 << ADSC);
                adc_state = ADC_CONVERT;
            }
            break;
            
        case ADC_CONVERT:
            if (!(ADCSRA & (1 << ADSC))) {
                adc_state = ADC_READY;
            }
            break;
            
        case ADC_READY:
            tensiune_raw = (uint32_t)ADC_Get_Result() * 5000UL / 1023UL;
            // Dupa ce avem valoarea raw, o trecem prin filtru
            fir_filter_process(tensiune_raw);
            adc_state = ADC_REPAUS;
            break;
    }
}

bool ADC_Is_Data_Ready(void) {
    if (data_ready_flag) {
        data_ready_flag = false; // Resetam flagul la citire
        return true;
    }
    return false;
}

uint16_t ADC_Get_Filtered_Value(void) {
    return tensiune_filtrata;
}