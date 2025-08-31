#pragma once

#include <atomic>

#include "hal/igpio.h"
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"

namespace hal::gpio {

class Stm32Gpio : public IGpio {
 public:
  Stm32Gpio(const Stm32Gpio&) = delete;
  Stm32Gpio(Stm32Gpio&&) = delete;
  Stm32Gpio& operator=(const Stm32Gpio&) = delete;
  Stm32Gpio& operator=(Stm32Gpio&&) = delete;

  Stm32Gpio(GPIO_TypeDef* port, uint16_t pin, uint32_t speed = GPIO_SPEED_LOW);

  Error init(const Config cfg) override;
  Error deinit() override;
  bool is_initialized() override;
  Error reconfigure(const Config cfg) override;
  Error write_level(Level level) override;
  Error read_level(Level& level) override;
  Error set_direction(Direction dir) override;
  Error set_edge(Edge edge) override { return Error::Unsupported; }
  Error set_edge_trigger(Trigger trigger) override { return Error::Unsupported; }
  Error enable_interrupt(bool enable) override { return Error::Unsupported; }
  Error toggle() override;

  const Config& get_cfg() const override;

 private:
  GPIO_TypeDef* port;
  uint32_t speed;
  uint16_t pin;
  Config cfg;
  std::atomic<bool> inited = false;
};
}  // namespace hal::gpio
