#pragma once

#include <cstdint>
#include <functional>
#include <optional>

#include "hal/error.h"
#include "util/span/span.h"

namespace hal::spi {

enum class Mode : uint8_t {
  M0 = 0,
  M1 = 1,
  M2 = 2,
  M3 = 3,
};

enum class BitOrder : uint8_t {
  MsbFirst = 0,
  LsbFirst = 1,
};

struct Config {
  uint32_t frequency_hz = 1'000'000;
  Mode mode = Mode::M0;
  BitOrder bit_order = BitOrder::MsbFirst;
  uint8_t bits_per_word = 8;
  uint32_t default_timeout_ms = 100;
};

using CsControl = std::function<bool(bool active)>;

class ISpi {
 public:
  virtual ~ISpi() = default;

  /// @brief Initialize spi
  /// SPI won't be able to send or receive until initialization
  virtual Error init(const Config cfg) = 0;
  /// @brief Deinitialize spi
  /// Reverts all configuration
  virtual Error deinit() = 0;
  virtual Error reconfigure(const Config cfg) = 0;
  /// @brief True if initialized using a valid configuration
  virtual bool is_initialized() const = 0;
  /// @brief Atomic full-duplex transfer
  /// If 'tx_buffer' null: read-only mode
  /// If 'rx_buffer' null: write-only mode
  virtual Error transfer(const util::span::ByteSpan* tx_buffer, util::span::ByteSpan* rx_buffer,
                         uint32_t timeout_ms) = 0;

  /// @brief Shorthand for 'transfer' with 'default_timeout_ms' from configuration
  inline Error transfer(const util::span::ByteSpan* tx_buffer, util::span::ByteSpan* rx_buffer) {
    return transfer(tx_buffer, rx_buffer, cfg.default_timeout_ms);
  };
  /// @brief Shorthand for 'write' with 'default_timeout_ms' from configuration
  inline Error write(const util::span::ByteSpan& tx_buffer) { return write(tx_buffer, cfg.default_timeout_ms); }
  /// @brief Shorthand for 'transfer' in 'write-only' mode
  inline Error write(const util::span::ByteSpan& tx_buffer, uint32_t timeout_ms) {
    return transfer(&tx_buffer, nullptr);
  }
  /// @brief Shorthand for 'read" with 'default_timeout_ms' from configuration
  inline Error read(util::span::ByteSpan& rx_buffer) { return read(rx_buffer, cfg.default_timeout_ms); }
  /// @brief Shorthand for 'transfer' in 'read-only' mode
  inline Error read(util::span::ByteSpan& rx_buffer, size_t timeout_ms) {
    return transfer(nullptr, &rx_buffer, timeout_ms);
  }

  /// @brief Set custom chip select pin callback
  void set_cs_control(CsControl cs) { cs_ctrl.emplace(std::move(cs)); }
  /// @brief Get reference to configured chip select pin callback
  const std::optional<CsControl>& cs_control() const { return cs_ctrl; }

  /// @brief Trigger chip select active
  /// True if no callback or callback ran successfully
  bool cs_assert() { return !cs_ctrl.has_value() || (*cs_ctrl)(true); }
  /// @brief Trigger chip select inactive
  /// True if no callback or callback ran successfully
  bool cs_deassert() { return !cs_ctrl.has_value() || (*cs_ctrl)(false); }

 protected:
  Config cfg;
  std::optional<CsControl> cs_ctrl;
};

}  // namespace hal::spi
