#pragma once

#include <cstdint>
#include <functional>
#include <optional>

#include "hal/error.h"
namespace hal::gpio {

enum class Level : uint8_t {
  Low = 0,
  High = 1,
};

enum class State : uint8_t {
  Inactive = 0,
  Active = 1,
};

enum class Mode : uint8_t {
  ActiveLow,
  ActiveHigh,
};

enum class Pull : uint8_t {
  None,
  Up,
  Down,
};

enum class Direction : uint8_t {
  In,
  Out,
};

enum class Drive : uint8_t {
  PushPull,
  OpenDrain,
};

enum class Edge : uint8_t {
  None,
  Rising,
  Falling,
  Both,
};

struct Config {
  Direction dir = Direction::Out;
  Pull pull = Pull::None;
  Drive drive = Drive::PushPull;
  Mode mode = Mode::ActiveHigh;
  Level initial = Level::Low;
};

constexpr Level invert_level(Level l) { return l == Level::Low ? Level::High : Level::Low; }
constexpr Level state_to_level(State s, Mode m) {
  switch (m) {
    case Mode::ActiveLow:
      return s == State::Active ? Level::Low : Level::High;
    case Mode::ActiveHigh:
      return s == State::Active ? Level::High : Level::Low;
  }
}
constexpr State level_to_state(Level l, Mode m) {
  switch (m) {
    case Mode::ActiveLow:
      return l == Level::Low ? State::Active : State::Inactive;
    case Mode::ActiveHigh:
      return l == Level::High ? State::Active : State::Inactive;
  }
}

class IGpio {
 public:
  using Trigger = std::function<void()>;
  virtual ~IGpio() = default;

  virtual Error init(const Config cfg) = 0;
  virtual Error deinit() = 0;
  virtual Error reconfigure(const Config cfg) = 0;
  virtual bool is_initialized() = 0;
  virtual Error write_level(Level level) = 0;
  virtual Error read_level(Level& level) = 0;
  virtual Error set_direction(Direction dir) = 0;
  virtual Error set_edge(Edge edge) = 0;
  virtual Error set_edge_trigger(Trigger trigger) = 0;
  virtual Error enable_interrupt(bool enable) = 0;
  virtual Error toggle() = 0;

  /// @brief Get current configuration
  /// @note Mutating this won't change the actual config
  virtual const Config& get_cfg() const = 0;

  Error set_state(State state) { return write_level(state_to_level(state, get_cfg().mode)); }
  Error get_state(State& state) {
    Level l;
    if (Error e = read_level(l); e != Error::Ok) return e;
    state = level_to_state(l, get_cfg().mode);
    return Error::Ok;
  }
};
}  // namespace hal::gpio
