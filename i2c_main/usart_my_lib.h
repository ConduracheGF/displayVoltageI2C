/*--------------------------------------------------------------------------
* Fișier: mylib.h
* Utilizat pentru declararea funcției my_print
*------------------------------------------------------------------------*/
#ifndef __MYLIB__
#define __MYLIB__
/*--------------------------------------------------------------------------
* Includes
*------------------------------------------------------------------------*/
// Compiler
#include <stdint.h>
#include <inavr.h>
#include <ioavr.h>
// General
#include "usart.h"
/*--------------------------------------------------------------------------
* Data structures
*------------------------------------------------------------------------*/
typedef enum Tip {
  INTEGER,
  HEX,
  DOUBLE,
  CHAR
} Tipuri;
/*--------------------------------------------------------------------------
* Public (exported) functions
*---*/
/*
* Funcția de print care folosește USART
* Tip definește tipul variabilei: 0 = int, 1 = hexa, 2 = double, 3 = char
*/
void my_print(Tipuri tip, void *val);
#endif