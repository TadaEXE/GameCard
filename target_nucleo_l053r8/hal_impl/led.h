#pragma once
#include <stdint.h>

#include "main.h"
#include "hal/iled.h"
#include "stm32l053xx.h"

namespace hal_impl {
class Led : public hal::ILed {
 public:
  Led(GPIO_TypeDef* port, uint16_t pin) : port(port), pin(pin) {}
  void toggle() override;

 private:
  GPIO_TypeDef* port;
  uint32_t pin;
};
}  // namespace hal_impl
