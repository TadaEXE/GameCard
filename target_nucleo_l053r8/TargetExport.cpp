#include "hal/idelay.h"
#include "hal/iled.h"
#include "hal/names.h"
#include "hal_impl/delay.h"
#include "hal_impl/led.h"
#include "main.h"
#include "util/import.h"

template <>
hal::ILed& util::ImportNamed<hal::ILed, names::TEST_LED>::get() {
  static hal_impl::Led led{LD2_GPIO_Port, LD2_Pin};
  return led;
}

template <>
hal::IDelay& util::Import<hal::IDelay>::get() {
  static hal_impl::Delay delay{};
  return delay;
}
