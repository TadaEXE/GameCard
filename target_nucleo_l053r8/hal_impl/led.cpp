#include "led.h"

#include "stm32l0xx_hal_gpio.h"

namespace hal_impl {
void Led::toggle() {
  HAL_GPIO_TogglePin(port, pin);
}
}  // namespace hal_impl
