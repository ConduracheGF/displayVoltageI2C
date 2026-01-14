#include "functii_i2c.h"
#include "functii_uart.h"
#include "functii_adc.h"
#include "functii_timer.h"
#include "state_machine.h"

int main() {
    uint32_t suma_tensiune = 0;
    uint16_t counter = 0;

    USART_Init(UBRR);
    setup_i2c();
    ADC_Init();

    while (1){
        uint16_t adc = ADC_Read();
        uint16_t tensiune = (uint32_t)adc * 5000UL / 1023UL;
        
        suma_tensiune += tensiune;
        counter++;
        if(counter == 100){
            suma_tensiune /= 100;
            counter = 0;

            driver_task(suma_tensiune);

            USART_Transmit_String("Tensiune = ");
            USART_Transmit_Int(tensiune);
            USART_Transmit_String(" mV");
        }
    }
}
