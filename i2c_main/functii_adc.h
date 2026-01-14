#ifndef FUNCTII_ADC_H_
#define FUNCTII_ADC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ADC_Init(void);
uint16_t ADC_Read(void);

#ifdef __cplusplus
}
#endif

#endif