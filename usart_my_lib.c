#include "usart_my_lib.h"

// Buffer auxiliar pentru construirea șirului de caractere
static uint8_t aux1[10] = { 0 }, aux2[10] = { 0 };

// Funcția de transmitere a unui număr întreg pe serială
void integerTransmit(void *p) {
  // Se inițializează indicii pentru construcția șirului de caractere
  int16_t index = 0, i = 0;
  // Se preia valoarea întreagă din pointer
  int16_t x = (*(int16_t *)(p));
  // Se tratează cazul numerelor negative
  if (x < 0) {
    aux1[index] = '-';  // Se adaugă semnul minus
    index++;
    x *= (-1);
    // Se convertește la valoare pozitivă
  }
  // Se extrag cifrele numărului în ordine inversă
  do {
    // Se convertește cifra la caracter ASCII
    uint8_t c = x % 10 + '0';

    // Se stochează temporar în buffer auxiliar
    aux2[i] = c;
    i++;
    // Se reduce numărul
    x = x / 10;
  } while (x != 0);
  // Se copiază cifrele în ordinea corectă în bufferul final
  for (int16_t j = i - 1; j >= 0; j--) {
    aux1[index] = aux2[j];
    index++;
  }
  // Se adaugă caracterele de sfârșit de linie
  aux1[index] = '\n';
  index++;
  aux1[index] = '\r';
  index++;
  // Se transmite șirul format către modulul USART
  USART_transmit_string(aux1, index);
}

// Funcția de transmitere a unui număr hexazecimal pe serială
void hexadecimalTransmit(void *p) {
  // Se preia valoarea întreagă din pointer
  int16_t x = *((int16_t *)(p));
  int16_t index = 0, i = 0;
  // Se adaugă prefixul "0x" pentru format hexazecimal
  aux1[index] = '0';
  index++;
  aux1[index] = 'x';
  index++;
  // Se extrag cifrele hexazecimale în ordine inversă
  do {
    uint8_t a = x & 0x0F;  // Se preiau cei 4 biți cei mai mici
    // Se convertește valoarea la caracter ASCII
    if (a <= 9) {
      aux2[i] = a + '0';
    } else {
      aux2[i] = a + 'A' - 10;
    }
    i++;
    x >>= 4;  // Se face shiftare pentru următoarea cifră
  } while (x != 0);
  // Se copiază cifrele în ordinea corectă
  for (int16_t j = i - 1; j >= 0; j--) {
    aux1[index] = aux2[j];

    index++;
  }
  // Se transmite șirul format către modulul USART
  USART_transmit_string(aux1, index);
}

// Funcția de transmitere a unui număr de tip double pe serială
void doubleTransmit(void *p) {
  // Se preia valoarea double din pointer
  double x = (*(double *)(p));
  int16_t index = 0;
  int16_t i = 0;
  // Se tratează cazul numerelor negative
  if (x < 0) {
    aux1[index++] = '-';
    x = -x;
  }
  // Se separă partea întreagă de partea fracționară
  int16_t int_part = (int16_t)x;
  double frac = x - int_part;
  // Se păstrează două zecimale
  int16_t frac_part = (int16_t)(frac * 100 + 0.5);
  // Se procesează partea întreagă
  if (int_part == 0) {
    aux1[index++] = '0';
  } else {
    while (int_part != 0) {
      // Se convertește cifra la caracter ASCII
      uint8_t c = int_part % 10 + '0';
      // Se stochează temporar în buffer auxiliar
      aux2[i++] = c;
      // Se reduce valoarea
      int_part /= 10;
    }
    // Se copiază cifrele în ordinea corectă
    for (int16_t j = i - 1; j >= 0; j--) {
      aux1[index++] = aux2[j];
    }
  }
  // Se adaugă separatorul zecimal
  aux1[index++] = '.';
  i = 0;
  // Se procesează partea fracționară
  if (frac_part == 0) {
    aux1[index++] = '0';
    aux1[index++] = '0';
  }

  else {
    if (frac_part < 10) {
      aux1[index++] = '0';  // Se adaugă zeroul pentru o cifră
    }
    // Se extrag cifrele fracționare în ordine inversă
    while (frac_part != 0) {
      uint8_t c = frac_part % 10 + '0';
      aux2[i++] = c;
      frac_part /= 10;
    }
    // Se copiază cifrele în ordinea corectă
    for (int16_t j = i - 1; j >= 0; j--) {
      aux1[index++] = aux2[j];
    }
  }
  // Se adaugă caracterele de sfârșit de linie
  aux1[index++] = '\n';
  aux1[index++] = '\r';
  // Se transmite șirul format către modulul USART
  USART_transmit_string(aux1, index);
}
// Se transmite un caracter pe serială
void characterTransmit(void *p) {
  // Se preia caracterul din pointer
  int8_t x = (*(int8_t *)(p));
  int16_t index = 0;
  // Se adaugă caracterul în buffer
  aux1[index++] = x;
  // Se transmite caracterul către modulul USART
  USART_transmit_string(aux1, index);
}

// Funcție de tip wrapper prin care se transmit valori de diverse tipuri
void my_print(Tipuri tip, void *val) {
  switch (tip) {
    case INTEGER:
      // Se transmite un număr întreg
      integerTransmit(val);
      break;
    case HEX:
      // Se transmite un număr hexazecimal
      hexadecimalTransmit(val);
      break;


    case DOUBLE:
      // Se transmite un număr double
      doubleTransmit(val);
      break;
    case CHAR:
      // Se transmite un caracter
      characterTransmit(val);
      break;
  }
}