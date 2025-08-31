#pragma once

namespace hal {

enum class Error {
  Ok = 0,
  NotInitialized,
  Busy,
  Timeout,
  IOError,
  InvalidArg,
  Unsupported,
};

}
