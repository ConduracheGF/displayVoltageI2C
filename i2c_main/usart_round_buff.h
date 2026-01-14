#ifndef FUNCTII_UART_H_
#define FUNCTII_UART_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FOSC 16000000
#define FSCL 100000
#define BAUD_RATE 9600
#define UBRR FOSC / 16 / BAUD_RATE - 1

  void USART_Init(unsigned int ubrr);

  void USART_Transmit(unsigned char data);

  void USART_Transmit_String(const char *string);

  void USART_Transmit_Int(uint16_t string);

#ifdef __cplusplus
}
#endif

#endif


/*---------------------------------------------------------------------------
* Fișier: round_buff.h
* Utilizat pentru declararea funcțiilor și structurii bufferului circular
*-------------------------------------------------------------------------*/
#ifndef __ROUND_BUFF__
#define __ROUND_BUFF__
/*---------------------------------------------------------------------------
* Includes
*-------------------------------------------------------------------------*/
// Biblioteca standard
#include <stdint.h>
#include <stddef.h>
#include <inavr.h>
#include <ioavr.h>
/*---------------------------------------------------------------------------
* Public defines
*-------------------------------------------------------------------------*/
// Dimensiunea bufferului circular
#define BUFFER_SIZE 16
/*---------------------------------------------------------------------------
* Data structures
*-------------------------------------------------------------------------*/
// Structură utilizată pentru buffer circular
struct round_buff {
  uint8_t buffer[BUFFER_SIZE];  // Array-ul de date
  uint8_t head;
  // Indexul de citire
  uint8_t tail;
  // Indexul de scriere
};
/*---------------------------------------------------------------------------
* Type definitions
*-------------------------------------------------------------------------*/

// Tip definit pentru buffer circular
typedef struct round_buff round_buff_s;
/*---------------------------------------------------------------------------
* Public (exported) variables
*-------------------------------------------------------------------------*/
// Buffer folosit pentru transmisie
extern round_buff_s tx_buffer;
// Buffer folosit pentru recepție
extern round_buff_s rx_buffer;
/*---------------------------------------------------------------------------
* Public (exported) functions
*-------------------------------------------------------------------------*/
// Funcție folosită pentru adăugarea unui caracter în buffer
int16_t push(round_buff_s *in_buffer, uint8_t data);
// Funcție folosită pentru extragerea unui caracter din buffer
uint8_t pop(round_buff_s *in_buffer);
// Funcție folosită pentru adăugarea unui șir de caractere în buffer
int16_t push_vec(round_buff_s *in_buffer, uint8_t data[], int16_t length);
// Funcție folosită pentru verificarea dacă bufferul este gol
int16_t is_empty(round_buff_s *in_buffer);
// Funcție folosită pentru verificarea dacă bufferul este plin
int16_t is_full(round_buff_s *in_buffer);
#endif