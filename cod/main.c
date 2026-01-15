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

typedef enum {
  SYSTEM_OK,
  SYSTEM_ERROR
} system_status_t;

static system_status_t system_status = SYSTEM_OK;

int main(void) {
    
    // Initializare ADC (configurare registre, fara start conversie)
    ADC_Init();
    
    // Initializare I2C pentru comunicarea cu driverul de display
    i2c_init();
    
    // Initializare USART cu Baud Rate calculat (9600 bps)
    USART_initialize(BAUD_RATE);
    
    // Initializare Timer0 pentru baza de timp de 1ms
    Timer0_Init();
    
    // Activare globala a intreruperilor (necesar pentru I2C, USART si Timer)
    __enable_interrupt();

    // Variabila locala pentru a stoca valoarea de afisat/trimis
    uint16_t valoare_curenta = 0;

    while (1) {
        
        // Verificam daca au trecut 20ms (bataia inimii a sistemului)
        if (Timer_Check_20ms()) {
            
            // Management ADC: 
            // Task-ul gestioneaza singur Starile: Pornire -> Asteptare -> Citire -> Filtrare FIR
            ADC_Task_Run();
            
            // Verificam daca avem date noi de la ADC
            if (ADC_Is_Data_Ready()) {
                
                // Preluam valoarea filtrata (media ultimelor 8 masuratori)
                valoare_curenta = ADC_Get_Filtered_Value();
                
                // Management Display:
                // Trimite datele catre AS1115 via I2C. 
                // Daca I2C-ul este ocupat sau da eroare, task-ul va incerca sa re-initializeze driverul.
                Display_Task_Run(valoare_curenta);
                
                // Management USART (State Machine):
                // Trimitem pe seriala doar daca masina de stari USART este in IDLE 
                // (adica bufferul circular de transmisie a fost golit anterior)
                if (USART_Is_Ready()) {
                    my_print(INTEGER, &valoare_curenta);
                }
            }
            
            // Verificare Siguranta:
            // Daca driverul de afisare raporteaza erori critice repetate
            if (Display_Is_Error()) {
                system_status = SYSTEM_ERROR;
            } else {
                system_status = SYSTEM_OK;
            }
        }
        
        // Task-uri de fundal:
        // Acestea ruleaza cat mai des posibil (in afara if-ului de 20ms) 
        // pentru a procesa cozile de date sau flag-urile de hardware.
        USART_Task_Run(); 
    }
}