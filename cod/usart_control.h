#ifndef __USART_CONTROL__
#define __USART_CONTROL__

#include <inavr.h>
#include <ioavr.h>
#include <stdbool.h>
#include "usart_round_buff.h"

#define F_OSC 16000000UL
#define BAUD 9600
#define BAUD_RATE (F_OSC / 16 / BAUD - 1)

// Definirea stărilor interne pentru State Machine
typedef enum {
    USART_STATE_IDLE,
    USART_STATE_SENDING,
    USART_STATE_COMPLETE
} usart_task_state_t;


void USART_initialize(uint16_t baud_rate);
uint16_t USART_transmit_string(uint8_t *s, int16_t length);

bool USART_Task_Run(uint8_t *data, int16_t len);      // Rulează logica de stare
bool USART_Is_Ready(void);    // Verifică dacă putem trimite date noi

#endif