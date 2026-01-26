#ifndef FUNCTII_ADC_H_
#define FUNCTII_ADC_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  ADC_REPAUS,
  ADC_CONVERT,
  ADC_READY
} adc_state_t;

void ADC_Init(void);
uint16_t ADC_Get_Result(void);

// Functia principala de task, care trebuie apelata periodic
void ADC_Task_Run(void);

// Verifica daca avem o noua valoare filtrata gata
bool ADC_Is_Data_Ready(void);

// Returneaza ultima valoare filtrata calculata
uint16_t ADC_Get_Filtered_Value(void);

#endif