#include "usart_round_buff.h"

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
