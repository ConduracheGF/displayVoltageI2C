#include "control_system.h"
#include <ioavr.h>
#include <intrinsics.h>
#include <stdio.h>
#include <string.h>

#include "adc.h"
#include "usart_control.h"
#include "functii_timer.h"
#include "filtru_fir.h"
#include "functii_i2c.h"

static uint16_t k=0;
static sistem_state_t g_sistem_state = SISTEM_IDLE;
static char debug_buffer[64];
static uint16_t tensiune_mv_actuala = 0;

void Sistem_Init(void) {
    ADC_Init();
    USART_initialize(BAUD_RATE); // 9600 baud la 16MHz
    Timer0_Init();
    setup_i2c();
    
    //setup blocant strict pentru configurarea hardware a afisajului
    while (setup_7_segm() == I2C_STATUS_BUSY);
    
    __enable_interrupt();
    
    USART_transmit_string((uint8_t*)"Sistem Multitasking Online\r\n", 28);
    g_sistem_state = SISTEM_IDLE;
}

void Sistem_Task_Run(void) {
    //task-urile periferice trebuie sa ruleze permanent pentru a procesa intreruperi si buffere
    ADC_Task_Run();
    USART_Task_Run(NULL, 0);

    switch (g_sistem_state) {
        
        case SISTEM_IDLE:
            //asteptam declansarea la fiecare 20ms prin timer
            if (Timer_Check()) {
                g_sistem_state = SISTEM_CITIRE_ADC;
            }
            break;

        case SISTEM_CITIRE_ADC:
            //verificam daca masina de stari a ADC-ului a terminat conversia
            if (ADC_New_Data_Available()) {
                uint16_t raw = ADC_Get_Last_Value();
                k++;
                uint16_t filtru = FIR_Update(raw);
                
                //calculam tensiunea
                tensiune_mv_actuala = (uint16_t)(((uint32_t)filtru * 3300UL) >> 10);
                g_sistem_state = SISTEM_UPDATE_AFISAJ;
            }
            break;

        case SISTEM_UPDATE_AFISAJ:
            //trimitem datele catre I2C
            if (I2C_Task_Run(tensiune_mv_actuala, 0) != I2C_STATUS_BUSY) {
                g_sistem_state = SISTEM_TRIMITE_USART;
            }
            break;

        case SISTEM_TRIMITE_USART:
            //pregatim si trimitem raportul serial
            sprintf(debug_buffer, "V_SYS: %u mV\r\n", tensiune_mv_actuala);
            USART_transmit_string((uint8_t*)debug_buffer, strlen(debug_buffer));
            
            //ne intoarcem in IDLE pentru a astepta urmatoarea bifa de timer
            g_sistem_state = SISTEM_IDLE;
            break;

        default:
            g_sistem_state = SISTEM_IDLE;
            break;
    }
}