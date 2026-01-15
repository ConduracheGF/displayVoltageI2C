#include "usart_control.h"

/* Definirea macro-urilor pentru registrele USART1 */
#define SET_BAUD_H(BAUDRATE) UBRR1H = (uint8_t)(BAUDRATE >> 8)
#define SET_BAUD_L(BAUDRATE) UBRR1L = (uint8_t)(BAUDRATE & 0xFF)

// Definirea stărilor interne pentru State Machine
typedef enum {
    USART_STATE_IDLE,
    USART_STATE_SENDING,
    USART_STATE_COMPLETE
} usart_task_state_t;

static usart_task_state_t usart_state = USART_STATE_IDLE;

// Bufferele circulare sunt declarate extern în .h, aici le instanțiem
round_buff_s tx_buffer;
round_buff_s rx_buffer;

void USART_initialize(uint16_t baud_rate) {
    // Resetare indecși buffere
    tx_buffer.head = 0; tx_buffer.tail = 0;
    rx_buffer.head = 0; rx_buffer.tail = 0;

    // Configurare Baud Rate folosind macro-urile de mai sus
    SET_BAUD_H(baud_rate);
    SET_BAUD_L(baud_rate);
    
    // Activare Transmițător, Receptor și întreruperi
    // RXCIE1 - Intrerupere la recepție completă
    // TXCIE1 - Intrerupere la transmisie completă (folosită pentru buffer circular)
    UCSR1B = (1 << TXEN1) | (1 << RXEN1) | (1 << RXCIE1) | (1 << TXCIE1);
    
    usart_state = USART_STATE_IDLE;
}

void USART_Task_Run(void) {
    switch (usart_state) {
        case USART_STATE_IDLE:
            // Dacă buffer-ul nu mai este gol, înseamnă că am început să trimitem ceva
            if (!is_empty(&tx_buffer)) {
                usart_state = USART_STATE_SENDING;
            }
            break;

        case USART_STATE_SENDING:
            // Dacă buffer-ul s-a golit, marcăm finalizarea
            if (is_empty(&tx_buffer)) {
                usart_state = USART_STATE_COMPLETE;
            }
            break;

        case USART_STATE_COMPLETE:
            // Revenim în IDLE pentru a permite o nouă comandă de print
            usart_state = USART_STATE_IDLE;
            break;
    }
}

bool USART_Is_Ready(void) {
    return (usart_state == USART_STATE_IDLE);
}

uint16_t USART_transmit_string(uint8_t *s, int16_t length) {
    uint16_t res = push_vec(&tx_buffer, s, length);
    
    // Dacă transmițătorul este liber (UDRE1), forțăm trimiterea primului caracter 
    // pentru a declanșa cascada de întreruperi TX
    if (UCSR1A & (1 << UDRE1)) {
        if (!is_empty(&tx_buffer)) {
            UDR1 = pop(&tx_buffer);
        }
    }
    return res;
}

/* Întrerupere pentru Transmisie Completă */
#pragma vector = USART1_TX_vect
__interrupt void USART1_TX_ISR(void) {
    if (!is_empty(&tx_buffer)) {
        UDR1 = pop(&tx_buffer);
    }
}

/* Întrerupere pentru Recepție Completă (opțional, dar bun pentru stabilitate) */
#pragma vector = USART1_RX_vect
__interrupt void USART1_RX_ISR(void) {
    uint8_t data = UDR1;
    push(&rx_buffer, data);
}