#include "functii_uart.h"
#include <avr/io.h>

void USART_Init(unsigned int ubrr) {
  //setare high
  UBRR0H = (unsigned char)(ubrr >> 8);
  //setare low
  UBRR0L = (unsigned char)(ubrr);
  //activare transmit si receive
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  //format 8biti, fara par, 2 biti stop, mod asincron
  UCSR0C = (1 << USBS0) | (3 << UCSZ00) | (0 << UMSEL00) | (0 << UPM00);
}

void USART_Transmit(unsigned char data) {
  //buffer gol
  while (!(UCSR0A & (1 << UDRE0))) {}
  //incarci caracter
  UDR0 = data;
}

void USART_Transmit_String(const char *string) {
  //transmitere caracter cu caracter
  for (int i = 0; string[i] != '\0'; i++) {
    USART_Transmit(string[i]);
  }
  USART_Transmit('\r');
  USART_Transmit('\n');
}

void USART_Transmit_Int(uint16_t string) {
  char buff[32];
  int i = 0;

  if (string == 0) {
    USART_Transmit('0');
    return;
  }
  while (string) {
    buff[i++] = ((string % 10) + '0');
    string /= 10;
  }
  while (i--) {
    USART_Transmit(buff[i]);
  }
}

/*--------------------------------------------------------------------------
* Fișier: round_buff.c
* Utilizat pentru definirea funcțiilor și structurilor din round_buff.h
*------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
* Includes
*------------------------------------------------------------------------*/
// Include fișierul de header pentru buffer circular
#include "roundBuff.h"
/*--------------------------------------------------------------------------
* Public functions
*------------------------------------------------------------------------*/
// Funcție folosită pentru adăugarea unui caracter în bufferul circular
int16_t push(round_buff_s *inBuffer, uint8_t data) {
  /*
* Se calculează poziția următoare a indicelui head folosind operația
* modulo pentru comportament circular.
*/
  uint8_t next_head = (inBuffer->head + 1) % BUFFER_SIZE;
  /*
* Dacă bufferul nu este plin, se inserează caracterul și
* se actualizează indicele head.
*/
  if (!is_full(inBuffer)) {
    inBuffer->buffer[inBuffer->head] = data;
    inBuffer->head = next_head;
    // Returnează 1 pentru succes
    return 1;
  } else
    // Returnează 0 dacă bufferul este plin
    return 0;
}
// Funcție folosită pentru extragerea unui caracter din buffer
uint8_t pop(round_buff_s *inBuffer) {
  /*
* Dacă bufferul nu este gol, se preia caracterul din poziția tail
* și se actualizează indicele tail pentru comportament circular.
*/
  if (!is_empty(inBuffer)) {
    uint8_t data = inBuffer->buffer[inBuffer->tail];
    inBuffer->tail = (inBuffer->tail + 1) % BUFFER_SIZE;
    // Returnează caracterul extras
    return data;
  } else {
    // Returnează 0 dacă bufferul este gol
    return 0;
  }
}
// Funcție folosită pentru adăugarea unui șir de caractere în buffer
int16_t push_vec(round_buff_s *inBuffer, uint8_t data[], int16_t length) {
  int16_t i;
  /*
* Se parcurge fiecare caracter din șir și se încearcă adăugarea lui.
* În caz că bufferul se umple, se oprește inserarea.
*/
  for (i = 0; i < length; i++) {
    uint8_t verif = push(inBuffer, data[i]);
    if (verif == 0)
      break;
  }

  // Returnează numărul de caractere adăugate cu succes
  return i;
}
// Funcție folosită pentru verificarea dacă bufferul este gol
int16_t is_empty(round_buff_s *inBuffer) {
  return (inBuffer->head == inBuffer->tail);
}
// Funcție folosită pentru verificarea dacă bufferul este plin
int16_t is_full(round_buff_s *inBuffer) {
  return ((inBuffer->head + 1) % BUFFER_SIZE) == inBuffer->tail;
}
