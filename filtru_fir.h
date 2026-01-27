#ifndef FILTRU_FIR_H
#define FILTRU_FIR_H

#include <stdint.h>

#define FILTER_TAP_NUM 64
#define FILTER_MASK (FILTER_TAP_NUM - 1)
#define FILTER_SHIFT 6

void FIR_Init(void);
uint16_t FIR_Update(uint16_t new_sample);

#endif