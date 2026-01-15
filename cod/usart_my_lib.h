#ifndef __USART_MY_LIB__
#define __USART_MY_LIB__

#include <stdint.h>
#include <inavr.h>
#include <ioavr.h>
#include "usart_control.h"

typedef enum Tip {
  INTEGER,
  HEX,
  DOUBLE,
  CHAR
} Tipuri;
/*
* Funcția de print care folosește USART
* Tip definește tipul variabilei: 0 = int, 1 = hexa, 2 = double, 3 = char
*/
void my_print(Tipuri tip, void *val);
#endif