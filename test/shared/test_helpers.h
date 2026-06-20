/////////////////////////////////////////////////////////////////
/*
  Shared test helpers for AnalogButtons test suites.
  Simulates analog pin readings to drive AnalogButtons without hardware.
*/
/////////////////////////////////////////////////////////////////

#pragma once

#include <Arduino.h>
#include <Button2.h>
#include <AnalogButtons.h>

/////////////////////////////////////////////////////////////////

#define TEST_PIN        14
#define DEBOUNCE_MS     (BTN_DEBOUNCE_MS + 5)

#define BTN_A_VALUE     100
#define BTN_B_VALUE     500
#define BTN_C_VALUE     900

/////////////////////////////////////////////////////////////////

static uint16_t simulatedAnalogValue = 0;

uint16_t mockAnalogRead(byte pin) {
  return simulatedAnalogValue;
}

/////////////////////////////////////////////////////////////////

inline AnalogButtons createTestButtons() {
  AnalogButtons btns(TEST_PIN);
  btns.setAnalogReadFunction(mockAnalogRead);
  return btns;
}

/////////////////////////////////////////////////////////////////

inline void pressAnalog(AnalogButtons& btns, uint16_t value, unsigned long duration) {
  simulatedAnalogValue = value;
  unsigned long end = millis() + duration;
  while (millis() < end) {
    btns.loop();
    delay(1);
  }
}

inline void releaseAnalog(AnalogButtons& btns) {
  simulatedAnalogValue = 0;
  delay(5);
  btns.loop();
  delay(5);
  btns.loop();
}

inline void clickAnalog(AnalogButtons& btns, uint16_t value, unsigned long duration = DEBOUNCE_MS) {
  pressAnalog(btns, value, duration);
  releaseAnalog(btns);
}

/////////////////////////////////////////////////////////////////
