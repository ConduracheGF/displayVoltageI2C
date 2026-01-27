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
  // REFS1=0, REFS0=1 | MUX4:0 = 00000 (ADC0)
  ADMUX = (1 << REFS0);
  ADCSRB &= ~(1 << MUX5);
  // Reduce zgomotul si consumul pe pinul analogic
  DIDR0 |= (1 << ADC0D);
  // Prescaler 128 (la 16MHz avem 125kHz, optim pentru precizie)
  ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADIE);  
  adc_state = ADC_REPAUS;
}

uint16_t ADC_Get_Result(void){
  
    uint16_t val = ADCL;
    val |= (ADCH << 8);
    return val;
  
  //return ADC;
}

//FIR
static void fir_filter_process(uint16_t raw_val) {
    // Scadem valoarea veche care iese din buffer si adunam pe cea noua
    suma_activa = suma_activa - buffer_fir[idx] + raw_val;
    
    buffer_fir[idx] = raw_val;
    idx = (idx + 1) % 8;
    
    uint16_t medie_raw = (uint16_t)(suma_activa);
    
    //impartirea la 8 poli
    tensiune_filtrata = (uint16_t)((uint32_t)medie_raw * 3300UL / 1023UL);
    //semnalam ca datele sunt noi
    data_ready_flag = true;
}

void ADC_Task_Run(void) {
    switch (adc_state) {
        case ADC_REPAUS:
            //pornim o noua conversie doar daca flag-ul a fost consumat
            if (!data_ready_flag) {
                // REPORNIRE: Activam modulul ADC înainte de conversie
                ADCSRA |= (1 << ADEN);
                
                //START:Pornim conversia
                ADCSRA |= (1 << ADSC);
                adc_state = ADC_CONVERT;
            }
            break;
            
        case ADC_CONVERT:
            // Asteptam ca ISR sa finalizeze conversia si sa opreasca ADC-ul
            break;
            
        case ADC_READY:
            //tensiune_raw = ADC_Get_Result();
            //tensiune_raw = (uint32_t)ADC_Get_Result() * 5000UL / 1023UL;
            //OPRIRE: Dezactivam ADC-ul imediat pentru a economisi energie
            //si pentru a izola pinul de circuitele de conversie între citiri
            // dupa ce avem valoarea raw, o trecem prin filtru
            fir_filter_process(tensiune_raw);
            adc_state = ADC_REPAUS;
            break;
    }
}

/**
 se executa dupa terminarea conversiei (13 sau 25 cicluri)

#pragma vector = ADC_vect
__interrupt void ADC_ISR(void) {
    tensiune_raw = ADC;
    //tensiune_raw |= (ADCH << 8);            // Citim valoarea imediat
    
    // OPRIRE ADC: Dezactivam modulul imediat dupa achizitie
    ADCSRA &= ~(1 << ADEN);
    
    adc_state = ADC_READY;         // Trecem in starea de procesare
}
*/
bool ADC_Is_Data_Ready(void) {
    if (data_ready_flag) {
        data_ready_flag = false; //resetam flagul la citire
        return true;
    }
    return false;
}

uint16_t ADC_Get_Filtered_Value(void) {
    return tensiune_filtrata;
}