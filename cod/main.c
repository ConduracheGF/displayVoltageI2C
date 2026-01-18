#include <intrinsics.h>
#include <ioavr.h>
#include <stdint.h>
#include <stdbool.h>

#include "functii_i2c.h"
#include "usart_control.h"
#include "usart_my_lib.h"
#include "functii_adc.h"
#include "functii_afisare.h"
#include "functii_timer.h"

/*

typedef enum {
    SYSTEM_OK,
    SYSTEM_ERROR
} system_status_t;

static system_status_t system_status = SYSTEM_OK;

int main(void) {
    ADC_Init();
    i2c_init();
    USART_initialize(BAUD_RATE);
    Timer0_Init();
    
    __enable_interrupt();

    uint16_t valoare_curenta = 0;

    while (1) {
        // Ruleaza masina de stari ADC (Pornire -> Convert -> Filtrare)
        // Trebuie sa fie in afara timerului pentru a avea media FIR gata rapid
        ADC_Task_Run();
        
        // Ruleaza masina de stari USART (Trimite octetii din buffer pe rand)
        USART_Task_Run();

        if (Timer_Check_20ms()) {
            
            // Verificam daca ADC-ul a finalizat calculul mediei
            if (ADC_Is_Data_Ready()) {
                valoare_curenta = ADC_Get_Filtered_Value();

                // Nu mai folosim USART_Is_Ready() pentru a nu sari peste valori
                my_print(INTEGER, &valoare_curenta);

                // Verificam daca magistrala nu este blocata inainte de a trimite
                if (!i2c_is_busy()) {
                    Display_Task_Run(valoare_curenta);
                }
            }

            // Daca I2C raporteaza eroare, sistemul trece in status ERROR
            if (Display_Is_Error()) {
                system_status = SYSTEM_ERROR;
            } else {
                system_status = SYSTEM_OK;
            }
        }
    }
}

*/

extern volatile i2c_state_t g_i2c_state; 

void main(void) {
    // 1. Initializari Hardware
    i2c_init();
    Timer0_Init();
    __enable_interrupt();

    // 2. Initializare Driver Afisaj (asteptam pana reuseste)
    while(setup_7_segm() != SEND_SUCCESSFUL) {
        // Daca nu reuseste, resetam variabila de stare pentru a reincerca
        g_i2c_state = I2C_STATE_IDLE;
        delay_milisecunde(10); 
    }

    // 3. Datele de test
    uint16_t valori[] = {3245, 3976, 2578, 1234, 1678, 2345, 3456, 4567, 3245, 3976, 2578, 1234, 1678, 2345, 3456, 4567, 3245, 3976, 2578, 1234, 1678, 2345, 3456, 4567};
    uint8_t index = 0;

    while (1) {
        // Verificam daca a trecut timpul setat in Timer_Check (ex: 500ms)
        if (Timer_Check()) {
            
            // Verificam daca masina de stari I2C este libera
            if (!i2c_is_busy()) {
                // Trimitem valoarea la afisaj
                afisare_tensiune(valori[index]);
                
                // Trecem la urmatoarea valoare din vector
                index++;
                if (index >= 24) index = 0;
            } 
            else {
                /* LOGICA DE DEBLOCARE (Safety Net):
                   Daca am intrat aici, inseamna ca Timer_Check() a zis "e timpul", 
                   dar I2C-ul este inca blocat de la transmisia anterioara.
                */
                
                // Reset Hardware: Trimitem un semnal STOP fortat pe fire
                TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); 
                
                // Reset Software: Spunem codului ca magistrala este acum IDLE
                g_i2c_state = I2C_STATE_IDLE; 
            }
        }
        
        // Aici poti adauga si alte task-uri (ex: ADC_Task_Run() sau USART_Task_Run())
    }
}