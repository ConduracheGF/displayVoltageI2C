#include "control_system.h"

static uint16_t n=0;

/**
 * Punctul de intrare in aplicatie.
 * Main-ul are doar rolul de a initializa sistemul si de a porni
 * bucla infinita care "pulseaza" automatele de stari.
 */
void main(void) {
    
    //configurare ADC, USART, I2C, Timer
    //se activeaza si intreruperile globale.
    Sistem_Init();

    while (1) {
        n++;
        //aceasta va gestiona trecerea prin IDLE -> ADC -> I2C -> USART
        //non-blocant.
        Sistem_Task_Run();
        
    }
}