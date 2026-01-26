#include "control_system.h"
#include "usart_control.h"
#include "functii_adc.h"
#include "functii_timer.h"
#include "functii_i2c.h"
#include <stdio.h>

static sistem_state_t g_sistem_state = SISTEM_IDLE;
static char g_buffer_usart[64];
static uint16_t g_valoare_mv = 0;
static int16_t g_lungime_mesaj = 0;

void Sistem_Init(void) {
    USART_initialize(BAUD_RATE);
    ADC_Init();
    Timer0_Init();
    setup_i2c();
    
    // Ini?ializare fizica afi?aj (blocant doar la pornire)
    while (setup_7_segm() == I2C_STATUS_BUSY);
}

void Sistem_Task_Run(void) {
    // Task-urile hardware ruleaza mereu pentru a asigura continuitatea
    ADC_Task_Run();
    USART_Task_Run(NULL, 0); 

    switch (g_sistem_state) {
        case SISTEM_IDLE:
            // A?teptam semnalul de la Timer (200ms)
            if (Timer_Check()) {
                g_sistem_state = SISTEM_CITIRE_ADC;
            }
            break;

        case SISTEM_CITIRE_ADC:
            if (ADC_Is_Data_Ready()) {
                g_valoare_mv = ADC_Get_Filtered_Value();
                
                // Formatam string-ul cu virgula (Punct Fix)
                g_lungime_mesaj = sprintf(g_buffer_usart, 
                                         "Tensiune: %u.%03u V\r\n", 
                                         g_valoare_mv / 1000, 
                                         g_valoare_mv % 1000);
                
                g_sistem_state = SISTEM_UPDATE_AFISAJ;
            }
            break;

        case SISTEM_UPDATE_AFISAJ:
            // Actualizam afi?ajul I2C
            I2C_Task_Run(g_valoare_mv, 0);
            g_sistem_state = SISTEM_TRIMITE_USART;
            break;

        case SISTEM_TRIMITE_USART:
            // Încercam sa trimitem pe seriala doar daca portul e liber
            if (USART_Is_Ready()) {
                if (USART_Task_Run((uint8_t*)g_buffer_usart, g_lungime_mesaj)) {
                    g_lungime_mesaj = 0; // Resetam bufferul local
                    g_sistem_state = SISTEM_IDLE;
                }
            }
            break;
    }
}