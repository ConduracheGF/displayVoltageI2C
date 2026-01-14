#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  I2C_OK,
  I2C_STUCK
} i2c_bus_state_t;

typedef enum {
  DRIVER_OK,
  DRIVER_NEEDS_INIT
} driver_state_t;

typedef enum {
  SEND_SUCCESSFUL,
  SEND_NOT_SUCCESSFUL
} send_status_t;

typedef enum {
  TIMER_COUNTER,
  TIMER_WAIT,
  TIMER_EXPIRED
} timer_state_t

typedef enum {
  USART_IDLE,
  USART_SENDING,
  USART_COMPLETE
} usart_state_t

typedef enum {
  ADC_REPAUS,
  ADC_CONVERT,
  ADC_READY
} adc_state_t

typedef enum {
  FILTER_EMPTY,
  FILTER_READY
} fir_state_t

typedef enum {
  SYSTEM_OK,
  SYSTEM_WARNING,
  SYSTEM_ERROR
} system_state_t

extern adc_state_t adc_state;
extern driver_state_t driver_state;
extern fir_state_t fir_state;
extern i2c_bus_state_t i2c_bus_state;
extern system_state_t system_state;
extern timer_state_t timer_state;
extern usart_state_t usart_state;

void adc_task();
void driver_task(uint16_t tensiune);
void fir_task();
void i2c_task();
void system_control();
void timer_task();
void usart_task();

#ifdef __cplusplus
}
#endif

#endif

