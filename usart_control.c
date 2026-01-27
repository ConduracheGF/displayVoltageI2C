#include "usart_control.h"

/*definirea macrourilor pentru registrele USART1 */
#define SET_BAUD_H(BAUDRATE) UBRR1H = (uint8_t)(BAUDRATE >> 8)
#define SET_BAUD_L(BAUDRATE) UBRR1L = (uint8_t)(BAUDRATE & 0xFF)

static usart_task_state_t usart_state = USART_STATE_IDLE;

//bufferele circulare sunt declarate extern in header, aici le instanțiem
round_buff_s tx_buffer;
round_buff_s rx_buffer;

void USART_initialize(uint16_t baud_rate) {
    //resetare indecsi buffere
    tx_buffer.head = 0; tx_buffer.tail = 0;
    rx_buffer.head = 0; rx_buffer.tail = 0;

    //configurare baud rate folosind macro-urile de mai sus
    SET_BAUD_H(baud_rate);
    SET_BAUD_L(baud_rate);
    
    //activare transmitator, receptor si intreruperi
    //RXCIE1 - intrerupere la recepție completa
    //TXCIE1 - intrerupere la transmisie completa (folosita pentru buffer circular)
    UCSR1B = (1 << TXEN1) | (1 << RXEN1) | (1 << RXCIE1) | (1 << TXCIE1);
    
    usart_state = USART_STATE_IDLE;
}

bool USART_Task_Run(uint8_t *data, int16_t len) {
    bool ready = false;
  
    switch (usart_state) {
        case USART_STATE_IDLE:
            if (data != NULL && len > 0) {
                //incarcam datele in bufferul circular
                push_vec(&tx_buffer, data, len);
                
                // Fortam pornirea transmisiei daca hardware-ul e liber
                if (UCSR1A & (1 << UDRE1)) {
                    if (!is_empty(&tx_buffer)) {
                        UDR1 = pop(&tx_buffer);
                    }
                }
                usart_state = USART_STATE_SENDING;
            } else {
                ready = true; //suntem in IDLE și nu avem nimic de trimis
            }
            break;

        case USART_STATE_SENDING:
            //verificam daca bufferul s-a golit prin ISR
            if (is_empty(&tx_buffer)) {
                usart_state = USART_STATE_COMPLETE;
            }
            break;

        case USART_STATE_COMPLETE:
            usart_state = USART_STATE_IDLE;
            ready = true;
            break;
    }
    return ready;
}

bool USART_Is_Ready(void) {
    return (usart_state == USART_STATE_IDLE);
}

uint16_t USART_transmit_string(uint8_t *s, int16_t length) {
    //dezactivam intreruperile global pentru a proteja bufferul
    __disable_interrupt(); 
    
    uint16_t res = push_vec(&tx_buffer, s, length);
    
    // verificam daca transmitatorul este liber (UDRE1)
    if (UCSR1A & (1 << UDRE1)) {
        if (!is_empty(&tx_buffer)) {
            UDR1 = pop(&tx_buffer);
        }
    }
    
    // Reactivăm întreruperile
    __enable_interrupt(); 
    
    return res;
}

/*intrerupere pentru transmisie completa */
#pragma vector = USART1_TX_vect
__interrupt void USART1_TX_ISR(void) {
    if (!is_empty(&tx_buffer)) {
        UDR1 = pop(&tx_buffer);
    }
}

/*intrerupere pentru recepție completa*/
#pragma vector = USART1_RX_vect
__interrupt void USART1_RX_ISR(void) {
    uint8_t data = UDR1;
    push(&rx_buffer, data);
}