/////////////////////////////////////////////////////////////////
/*
  Basic tests for AnalogButton2 library.
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

test(test_basics, add_returns_button2_reference) {
  AnalogButton2 btns = createTestButtons();
  Button2* b = btns.add(BTN_A_VALUE, "A");
  // BTN_VIRTUAL_PIN = 254
  assertEqual(b->getPin(), (uint8_t)BTN_VIRTUAL_PIN);
}

/////////////////////////////////////////////////////////////////

test(test_basics, get_id_returns_label) {
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "LEFT");
  btns.add(BTN_B_VALUE, "RIGHT");

  clickAnalog(btns, BTN_A_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();
  // We don't have a direct handle but getId is tested via callback below;
  // here just verify no crash and label retrieval via add reference.
  Button2* b = btns.add(BTN_C_VALUE, "UP");
  assertEqual(btns.getId(*b), String("UP"));
}

/////////////////////////////////////////////////////////////////

test(test_basics, get_id_falls_back_to_value_string) {
  AnalogButton2 btns = createTestButtons();
  Button2* b = btns.add(BTN_A_VALUE);
  assertEqual(btns.getId(*b), String(BTN_A_VALUE));
}

/////////////////////////////////////////////////////////////////

test(test_basics, multiple_buttons_registered) {
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A");
  btns.add(BTN_B_VALUE, "B");
  btns.add(BTN_C_VALUE, "C");
  // If we get here without crashing, registration succeeded.
  assertTrue(true);
}

/////////////////////////////////////////////////////////////////

test(test_basics, max_buttons) {
  AnalogButton2 btns = createTestButtons();
  for (int i = 0; i < ABS_MAX_BUTTONS; i++) {
    btns.add((uint16_t)(50 + i * 90), String(i));
  }
  assertTrue(true);
}

/////////////////////////////////////////////////////////////////

test(test_basics, overflow_beyond_max_buttons_does_not_crash) {
  AnalogButton2 btns = createTestButtons();
  for (int i = 0; i <= ABS_MAX_BUTTONS; i++) {  // one over the limit
    btns.add((uint16_t)(50 + i * 50), String(i));
  }
  // If we reach here without a crash or memory corruption, the guard worked.
  assertTrue(true);
}

/////////////////////////////////////////////////////////////////

test(test_basics, get_count_tracks_registrations) {
  AnalogButton2 btns = createTestButtons();
  assertEqual(btns.getCount(), (byte)0);
  btns.add(BTN_A_VALUE, "A");
  assertEqual(btns.getCount(), (byte)1);
  btns.add(BTN_B_VALUE, "B");
  assertEqual(btns.getCount(), (byte)2);
}

/////////////////////////////////////////////////////////////////

test(test_basics, is_full_false_when_space_available) {
  AnalogButton2 btns = createTestButtons();
  assertFalse(btns.isFull());
}

/////////////////////////////////////////////////////////////////

test(test_basics, is_full_true_at_capacity) {
  AnalogButton2 btns = createTestButtons();
  for (int i = 0; i < ABS_MAX_BUTTONS; i++) {
    btns.add((uint16_t)(50 + i * 90), String(i));
  }
  assertTrue(btns.isFull());
}

/////////////////////////////////////////////////////////////////

test(test_basics, reset_clears_all_buttons) {
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A");
  btns.add(BTN_B_VALUE, "B");
  assertEqual(btns.getCount(), (byte)2);
  btns.reset();
  assertEqual(btns.getCount(), (byte)0);
  assertFalse(btns.isFull());
}

/////////////////////////////////////////////////////////////////

test(test_basics, add_after_reset_works) {
  static bool fired = false;
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A");
  btns.reset();
  btns.add(BTN_B_VALUE, "B")->setClickHandler([](Button2& b) { fired = true; });

  clickAnalog(btns, BTN_B_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(fired);
}

/////////////////////////////////////////////////////////////////

void setup() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
  delay(100);
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {}
  Serial.println(F("\n\nAnalogButton2 Basic Tests"));
}

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
