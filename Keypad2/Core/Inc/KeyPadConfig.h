#ifndef	_KEYPADCONFIG_H
#define	_KEYPADCONFIG_H
#include "gpio.h"

#define           _KEYPAD_DEBOUNCE_TIME_MS        20
#define           _KEYPAD_USE_FREERTOS            0

const GPIO_TypeDef* _KEYPAD_COLUMN_GPIO_PORT[] =
{
  GPIOA,
  GPIOC,
  GPIOC,
  GPIOC
};

const uint16_t _KEYPAD_COLUMN_GPIO_PIN[] =
{
  GPIO_PIN_8,
  GPIO_PIN_9,
  GPIO_PIN_8,
  GPIO_PIN_7
};

const GPIO_TypeDef* _KEYPAD_ROW_GPIO_PORT[] =
{
  GPIOA,
  GPIOA,
  GPIOA,
  GPIOA
};

const uint16_t _KEYPAD_ROW_GPIO_PIN[] =
{
  GPIO_PIN_12,
  GPIO_PIN_11,
  GPIO_PIN_10,
  GPIO_PIN_9
};

#endif
