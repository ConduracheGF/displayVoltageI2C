#include <inavr.h>
#include "control_system.h"

void main(void) {
    //configurare sistem
    Sistem_Init();

    __enable_interrupt();

    while (1) {
        //automatul de stari al sistemului
        Sistem_Task_Run();
    }
}