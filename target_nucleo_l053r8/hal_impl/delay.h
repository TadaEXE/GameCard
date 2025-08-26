#pragma once

#include <stdint.h>

#include "hal/idelay.h"

namespace hal_impl {

class Delay : public hal::IDelay {
 public:
  void delay(uint32_t ms) override;
};

}  // namespace hal_impl
