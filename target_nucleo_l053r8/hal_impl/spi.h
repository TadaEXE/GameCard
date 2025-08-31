#pragma once

#include <atomic>

#include "hal/ispi.h"
#include "stm32l0xx_hal.h"

namespace hal::spi {

class Stm32Spi : public spi::ISpi {
 public:
  explicit Stm32Spi(SPI_HandleTypeDef* handle) : handle(handle) {}

  Error init(const Config cfg) override;
  Error deinit() override;
  bool is_initialized() const override;
  Error reconfigure(const Config cfg) override;
  Error transfer(const util::span::ByteSpan* tx_buffer, util::span::ByteSpan* rx_buffer, uint32_t timeout_ms) override;

 private:
  SPI_HandleTypeDef* handle;
  std::atomic<bool> inited = false;
};

}  // namespace hal::spi
