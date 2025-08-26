#pragma once

namespace hal {

class ILed {
 public:
  virtual void toggle() = 0;
};

}  // namespace hal
