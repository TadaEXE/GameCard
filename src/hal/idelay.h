#pragma once

#include <stdint.h>
namespace hal {

class IDelay {
 public:
  virtual void delay(uint32_t ms) = 0;

  /// @brief Execute delay by using array access muhahahahha
  inline void operator[](uint32_t ms) { delay(ms); }
};
}  // namespace hal
