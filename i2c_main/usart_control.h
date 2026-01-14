/*----------------------------------------------------------------------------
* Fișier: usart.h
* Utilizat pentru declararea funcțiilor USART
*--------------------------------------------------------------------------*/
#ifndef __USART__
#define __USART__
/*----------------------------------------------------------------------------
* Includes
*--------------------------------------------------------------------------*/
#include <inavr.h>
#include <ioavr.h>
#include "roundBuff.h"
/*----------------------------------------------------------------------------
* Public defines
*--------------------------------------------------------------------------*/
#define F_OSC 16000000UL
#define BAUD 9600
#define BAUD_RATE (F_OSC / 16 / BAUD - 1)
/*-------------------------------------------------------------------------
* Public (exported) functions
*-----------------------------------------------------------------------*/
// Funcție folosită la inițializarea modulului USART
void USART_initialize(uint16_t baud_rate);
// Funcție folosită la ransmiterea unui singur caracter
uint16_t USART_transmit_char(uint8_t c);
// Funcție folosită la transmiterea a mai multor caractere
uint16_t USART_transmit_string(uint8_t *s, int16_t length);
// Funcție folosită pentru recepția unui singur caracter
uint16_t USART_receive_char(uint8_t *c);
// Funcție folosită pentru recepția a mai multor caractere
uint16_t USART_receive_string(uint8_t *c, uint16_t length);
// Funcție folosită la întreruperea pentru DATA REGISTER EMPTY
#pragma vector = USART1_TX_vect
__interrupt void USART1_TX_ISR(void);
// Funcție folosită la întreruperea pentru RECEIVE COMPLETE
#pragma vector = USART1_RX_vect
__interrupt void USART1_RX_ISR(void);
#endif