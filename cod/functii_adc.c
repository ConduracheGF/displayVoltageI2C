#include "functii_adc.h"
#include <ioavr.h>

static adc_state_t adc_state = ADC_REPAUS;
static uint16_t tensiune_raw = 0;
static uint16_t tensiune_filtrata = 0;
static bool data_ready_flag = false;

//Pentru filtrul fir pe 8 poli si 7 zerouri
static uint32_t buffer_fir[8] = {0};
static uint8_t idx = 0;
static uint32_t suma_activa = 0;

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
    // Scadem valoarea veche care iese din buffer si adunam pe cea noua
    suma_activa = suma_activa - buffer_fir[idx] + raw_val;
    
    buffer_fir[idx] = raw_val;
    idx = (idx + 1) % 8;
    
    //impartirea la 8 poli
    tensiune_filtrata = (uint16_t)(suma_activa >> 3); 
    data_ready_flag = true;
}

void ADC_Task_Run(void) {
    switch (adc_state) {
        case ADC_REPAUS:
            // Pornim o noua conversie doar daca flag-ul a fost consumat
            //if (!data_ready_flag) {
            ADCSRA |= (1 << ADSC);
            adc_state = ADC_CONVERT;
            //}
            break;
            
        case ADC_CONVERT:
            if (!(ADCSRA & (1 << ADSC))) {
                adc_state = ADC_READY;
            }
            break;
            
        case ADC_READY:
            tensiune_raw = ADC_Get_Result();
            //tensiune_raw = (uint32_t)ADC_Get_Result() * 5000UL / 1023UL;
            // Dupa ce avem valoarea raw, o trecem prin filtru
            fir_filter_process(tensiune_raw);
            tensiune_filtrata = (uint32_t)tensiune_filtrata * 5000UL / 1023UL;
            data_ready_flag = true;
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