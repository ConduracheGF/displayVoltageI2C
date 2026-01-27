#ifndef CONTROL_SYSTEM_H_
#define CONTROL_SYSTEM_H_

#include <stdint.h>
#include <stdbool.h>

/*
Enumerare pentru starile automatului de control global.
Gestioneaza fluxul de la citirea ADC pâna la raportarea pe USART.
*/
typedef enum {
    SISTEM_IDLE,            // Asteapta expirarea timer-ului
    SISTEM_CITIRE_ADC,      // Verifica si preia datele de la senzor
    SISTEM_UPDATE_AFISAJ,   // Actualizeaza afi?ajul 7-segmente via I2C
    SISTEM_TRIMITE_USART    // Formateaza si încarca string-ul în buffer-ul TX
} sistem_state_t;

/*
Initializeaza toate modulele periferice.
Configureaza USART, ADC, Timer0 si I2C.
Include si secventa de setup blocanta pentru driverul de afi?aj.
*/
void Sistem_Init(void);

/*
Functia principala de Task.
Trebuie apelata ciclic în bucla infinita din main.
Gestioneaza tranzi?iile între stari în mod non-blocant.
*/
void Sistem_Task_Run(void);

#endif