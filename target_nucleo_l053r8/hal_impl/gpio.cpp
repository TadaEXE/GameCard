#include "gpio.h"

namespace hal::gpio {

Stm32Gpio::Stm32Gpio(GPIO_TypeDef* port, uint16_t pin, uint32_t speed) : port(port), speed(speed), pin(pin) {}

Error Stm32Gpio::init(const Config cfg) {
  this->cfg = cfg;
  GPIO_InitTypeDef i{};
  i.Pin = pin;
  i.Mode = (cfg.dir == gpio::Direction::Out)
               ? (cfg.drive == gpio::Drive::OpenDrain ? GPIO_MODE_OUTPUT_OD : GPIO_MODE_OUTPUT_PP)
               : GPIO_MODE_INPUT;
  i.Pull = (cfg.pull == gpio::Pull::Up ? GPIO_PULLUP : cfg.pull == gpio::Pull::Down ? GPIO_PULLDOWN : GPIO_NOPULL);
  i.Speed = speed;
  HAL_GPIO_Init(port, &i);
  if (cfg.dir == Direction::Out) write_level(cfg.initial);

  return Error::Ok;
}

Error Stm32Gpio::deinit() {
  HAL_GPIO_DeInit(port, pin);
  return Error::Ok;
}

bool Stm32Gpio::is_initialized() { return inited; }

Error Stm32Gpio::reconfigure(const Config cfg) {
  if (Error e = deinit(); e != Error::Ok) return e;
  return init(cfg);
}

Error Stm32Gpio::write_level(Level level) {
  if (!inited) return Error::NotInitialized;
  if (cfg.dir != Direction::Out) return Error::InvalidArg;
  HAL_GPIO_WritePin(port, pin, level == Level::Low ? GPIO_PIN_RESET : GPIO_PIN_SET);
  return Error::Ok;
}

Error Stm32Gpio::read_level(Level& level) {
  if (!inited) return Error::NotInitialized;
  GPIO_PinState s = HAL_GPIO_ReadPin(port, pin);
  level = s == GPIO_PIN_SET ? Level::High : Level::Low;
  return Error::Ok;
}

Error Stm32Gpio::set_direction(Direction dir) {
  if (!inited) return Error::NotInitialized;
  cfg.dir = dir;
  return reconfigure(cfg);
}

// Error Stm32Gpio::set_edge(Edge edge) {}
//
// Error Stm32Gpio::set_edge_trigger(Trigger trigger) {}
//
// Error Stm32Gpio::enable_interrupt(bool enable) {}

const Config& Stm32Gpio::get_cfg() const { return cfg; }

Error Stm32Gpio::toggle() {
  if (!inited) return Error::NotInitialized;
  if (cfg.dir != Direction::Out) return Error::InvalidArg;
  HAL_GPIO_TogglePin(port, pin);
  return Error::Ok;
}

}  // namespace hal::gpio
