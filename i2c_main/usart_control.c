/*---------------------------------------------------------------------------
* Fișier: usart.c
* Utilizat pentru definirea funcțiilor USART din usart.h
*-------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
* Includes
*-------------------------------------------------------------------------*/
// General
#include "usart.h"
/*---------------------------------------------------------------------------
* (Private) defines
*-------------------------------------------------------------------------*/
/*
* Acest cod este configurat pentru portul microbus 1 de pe placă, folosind
* controalele UART1 ale procesorului.
*/
#define DATA_REGISTER_EMPTY
(UCSR1A &(1 << UDRE1))
#define SET_TRANSMITTER
  (1 << TXEN1)
#define SET_RECEIVER
    (1 << RXEN1)
#define SET_RX_COMPLETE
      (1 << RXCIE1)
#define SET_TX_COMPLETE
        (1 << TXCIE1)
#define SET_BAUD_H(BAUDRATE) UBRR1H = (uint8_t)(BAUDRATE >> 8)
#define SET_BAUD_L(BAUDRATE) UBRR1L = (uint8_t)(BAUDRATE & 0xFF)
  /*---------------------------------------------------------------------------
* Public variables
*-------------------------------------------------------------------------*/
  roundBuff_s tx_buffer;
roundBuff_s rx_buffer;
/*---------------------------------------------------------------------------
* Private functions
*-------------------------------------------------------------------------*/
void usart_send_next() {
  if (!isEmpty(&tx_buffer)) {
    // Se pune caracterul în registrul de date pentru a fi trimis
    UDR1 = pop(&tx_buffer);
  }
}
/*---------------------------------------------------------------------------
* Public functions
*-------------------------------------------------------------------------*/
// Funcție folosită la inițializarea modulului USART
void USART_initialize(uint16_t baud_rate) {
  // Se configurează baud rate-ul
  tx_buffer.head = 0;
  tx_buffer.tail = 0;
  rx_buffer.head = 0;
  rx_buffer.tail = 0;
  SET_BAUD_H(baud_rate);
  SET_BAUD_L(baud_rate);
  /*
* Se pornește transmițătorul și receptorul, iar apoi se activează
* întreruperea la recepție și la transmisie.
*/
  UCSR1B = SET_TRANSMITTER | SET_RECEIVER | SET_RX_COMPLETE | SET_TX_COMPLETE;
  // Se configurează paritatea pe par
  UCSR1C |= (1 << UPM11) | (1 << UCSZ11) | (1 << UCSZ10);
  // Se setează pinul TXD: ieșire
  DDRD |= (1 << PD3);
  // Se setează pinul RXD: intrare
  DDRD &= ~(1 << PD2);
}
// Funcție folosită la ransmiterea unui singur caracter
uint16_t USART_transmit_char(uint8_t c) {
  // Se dezactivează întreruperile pentru acces sigur la buffer
  __disable_interrupt();
  uint16_t verif;
  // Se verifică dacă caracterul nu este terminator de șir
  if (c != '\0')
    // Se adaugă caracterul în bufferul de transmisie
    verif = push(&tx_buffer, c);
  // Dacă registrul de date este gol și caracterul a fost adăugat cu succes
  if (DATA_REGISTER_EMPTY && verif != 0)
    // Se trimite imediat caracterul următor din buffer
    usart_send_next();
  // Se reactivează întreruperile
  __enable_interrupt();
  // Se returnează starea operației (0 = eșuat, 1 = reușit)
  return verif;
}

// Funcție folosită la transmiterea a mai multor caractere
uint16_t USART_transmit_string(uint8_t *s, int16_t length) {
  // Se dezactivează întreruperile pentru acces sigur la buffer
  __disable_interrupt();
  // Se adaugă șirul de caractere în bufferul de transmisie
  uint16_t verif = push_vec(&tx_buffer, s, length);
  // Dacă registrul de date e gol și caracterele au fost adăugate cu succes
  if (DATA_REGISTER_EMPTY && verif != 0)
    // Se trimite imediat primul caracter din buffer
    usart_send_next();
  // Se reactivează întreruperile
  __enable_interrupt();
  // Se returnează numărul de caractere adăugate cu succes în buffer
  return verif;
}
// Funcție folosită pentru recepția a mai multor caractere
uint16_t USART_receive_string(uint8_t *c, uint16_t length) {
  // Se dezactivează întreruperile pentru acces sigur la buffer
  __disable_interrupt();
  uint16_t i;
  // Se parcurge până la 'length' caractere (ceva evident în anul 3)
  for (i = 0; i < length; i++) {
    // Se scoate un caracter din bufferul de recepție
    uint8_t chr = pop(&rx_buffer);
    // Dacă bufferul e gol sau s-a ajuns la '\0', se oprește citirea
    if (chr == 0)
      break;
    // Se salvează caracterul citit în șirul furnizat de utilizator
    c[i] = chr;
  }
  // Se reactivează întreruperile
  __enable_interrupt();
  // Se returnează numărul de caractere citite
  return i;
}
/*
* Întrerupere pentru "Data Register Empty" (UDRIE), executată când
* transmițătorul este gata să trimită un nou caracter.
*/
#pragma vector = USART1_TX_vect
__interrupt void USART1_TX_ISR(void) {
  // Se trimite următorul caracter din bufferul de transmisie
  usart_send_next();
}

// Funcție folosită la transmiterea a mai multor caractere
uint16_t USART_transmit_string(uint8_t *s, int16_t length) {
  // Se dezactivează întreruperile pentru acces sigur la buffer
  __disable_interrupt();
  // Se adaugă șirul de caractere în bufferul de transmisie
  uint16_t verif = push_vec(&tx_buffer, s, length);
  // Dacă registrul de date e gol și caracterele au fost adăugate cu succes
  if (DATA_REGISTER_EMPTY && verif != 0)
    // Se trimite imediat primul caracter din buffer
    usart_send_next();
  // Se reactivează întreruperile
  __enable_interrupt();
  // Se returnează numărul de caractere adăugate cu succes în buffer
  return verif;
}
// Funcție folosită pentru recepția a mai multor caractere
uint16_t USART_receive_string(uint8_t *c, uint16_t length) {
  // Se dezactivează întreruperile pentru acces sigur la buffer
  __disable_interrupt();
  uint16_t i;
  // Se parcurge până la 'length' caractere (ceva evident în anul 3)
  for (i = 0; i < length; i++) {
    // Se scoate un caracter din bufferul de recepție
    uint8_t chr = pop(&rx_buffer);
    // Dacă bufferul e gol sau s-a ajuns la '\0', se oprește citirea
    if (chr == 0)
      break;
    // Se salvează caracterul citit în șirul furnizat de utilizator
    c[i] = chr;
  }
  // Se reactivează întreruperile
  __enable_interrupt();
  // Se returnează numărul de caractere citite
  return i;
}
/*
* Întrerupere pentru "Data Register Empty" (UDRIE), executată când
* transmițătorul este gata să trimită un nou caracter.
*/
#pragma vector = USART1_TX_vect
__interrupt void USART1_TX_ISR(void) {
  // Se trimite următorul caracter din bufferul de transmisie
  usart_send_next();
}

/*
* Întrerupere pentru "Receive Complete" (RXC), executată atunci când a sosit
* un caracter în registrul de recepție. Se preia caracterul din UDR1 și se
* stochează în bufferul de recepție.
*/
#pragma vector = USART1_RX_vect
__interrupt void USART1_RX_ISR(void) {
  // Se citește caracterul primit
  uint8_t received_char = UDR1;
  // Se adaugă caracterul în bufferul de recepție
  push(&rx_buffer, received_char);
}