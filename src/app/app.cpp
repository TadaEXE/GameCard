#include "app/app.h"

#include "hal/idelay.h"
#include "hal/iled.h"
#include "hal/names.h"
#include "util/import.h"

void start_app() {
  util::ImportNamed<hal::ILed, names::TEST_LED> led;
  util::Import<hal::IDelay> delay;

  while (true) {
    led->toggle();
    delay[100];
  }
}
