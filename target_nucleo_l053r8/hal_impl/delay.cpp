#include "delay.h"

#include <stdint.h>

#include "stm32l0xx_hal.h"

namespace hal_impl {

void Delay::delay(uint32_t ms) { HAL_Delay(ms); }

}  // namespace hal_impl
