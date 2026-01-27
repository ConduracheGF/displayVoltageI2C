#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Enumerare pentru starile automatului ADC
 */
typedef enum {
    ADC_REPAUS,   //ADC oprit (ADEN=0), asteapta declansarea
    ADC_CONVERT,  //conversie in curs, se asteapta intreruperea
    ADC_READY     //datele au fost citite si ADCul a fost oprit
} adc_state_t;

/*
initializeaza ADC: referinta AVCC, prescaler 128, 
activare intreruperi, dar ADCul e oprit (ADEN=0).
*/
void ADC_Init(void);

/*
masina de stari a ADC-ului. 
gestioneaza pornirea, asteptarea si pregatirea datelor.
*/
void ADC_Task_Run(void);

/*
verifica daca a fost finalizata o conversie noua.
true daca exista date noi, false altfel.
reseteaza flagul intern la citire.
*/
bool ADC_New_Data_Available(void);

/*
returneaza ultima valoare bruta (0-1023) citita de la ADC.
*/
uint16_t ADC_Get_Last_Value(void);

#endif // ADC_H