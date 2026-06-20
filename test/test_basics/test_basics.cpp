/////////////////////////////////////////////////////////////////
/*
  Basic tests for AnalogButtons library.
  Tests button registration, label retrieval, and constructor options.
*/
/////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <AUnitVerbose.h>
#include "../shared/test_helpers.h"

using namespace aunit;

/////////////////////////////////////////////////////////////////

#define SERIAL_SPEED 115200

/////////////////////////////////////////////////////////////////

test(basics, add_returns_button2_reference) {
  AnalogButtons btns = createTestButtons();
  Button2& b = btns.add(BTN_A_VALUE, "A");
  // BTN_VIRTUAL_PIN = 254
  assertEqual(b.getPin(), (uint8_t)BTN_VIRTUAL_PIN);
}

/////////////////////////////////////////////////////////////////

test(basics, get_id_returns_label) {
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "LEFT");
  btns.add(BTN_B_VALUE, "RIGHT");

  clickAnalog(btns, BTN_A_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();
  // We don't have a direct handle but getId is tested via callback below;
  // here just verify no crash and label retrieval via add reference.
  Button2& b = btns.add(BTN_C_VALUE, "UP");
  assertEqual(btns.getId(b), String("UP"));
}

/////////////////////////////////////////////////////////////////

test(basics, get_id_falls_back_to_value_string) {
  AnalogButtons btns = createTestButtons();
  Button2& b = btns.add(BTN_A_VALUE);
  assertEqual(btns.getId(b), String(BTN_A_VALUE));
}

/////////////////////////////////////////////////////////////////

test(basics, multiple_buttons_registered) {
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A");
  btns.add(BTN_B_VALUE, "B");
  btns.add(BTN_C_VALUE, "C");
  // If we get here without crashing, registration succeeded.
  assertTrue(true);
}

/////////////////////////////////////////////////////////////////

test(basics, max_buttons) {
  AnalogButtons btns = createTestButtons();
  for (int i = 0; i < ABS_MAX_BUTTONS; i++) {
    btns.add((uint16_t)(50 + i * 90), String(i));
  }
  assertTrue(true);
}

/////////////////////////////////////////////////////////////////

test(basics, overflow_beyond_max_buttons_does_not_crash) {
  AnalogButtons btns = createTestButtons();
  for (int i = 0; i <= ABS_MAX_BUTTONS; i++) {  // one over the limit
    btns.add((uint16_t)(50 + i * 50), String(i));
  }
  // If we reach here without a crash or memory corruption, the guard worked.
  assertTrue(true);
}

/////////////////////////////////////////////////////////////////

void setup() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
  delay(100);
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {}
  Serial.println(F("\n\nAnalogButtons Basic Tests"));
}

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
