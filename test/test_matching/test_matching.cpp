/////////////////////////////////////////////////////////////////
/*
  Value-matching tests for AnalogButtons library.
  Tests analog reading → button mapping, tolerance range, and unknown readings.
*/
/////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <AUnitVerbose.h>
#include "../shared/test_helpers.h"

using namespace aunit;

/////////////////////////////////////////////////////////////////

#define SERIAL_SPEED 115200

/////////////////////////////////////////////////////////////////

static bool btnA_fired = false;
static bool btnB_fired = false;

void resetMatchVars() {
  btnA_fired = false;
  btnB_fired = false;
  simulatedAnalogValue = 0;
}

/////////////////////////////////////////////////////////////////

test(test_matching, exact_value_triggers_button) {
  resetMatchVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setClickHandler([](Button2& b) { btnA_fired = true; });
  btns.add(BTN_B_VALUE, "B").setClickHandler([](Button2& b) { btnB_fired = true; });

  clickAnalog(btns, BTN_A_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
  assertFalse(btnB_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, value_within_range_triggers) {
  resetMatchVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setClickHandler([](Button2& b) { btnA_fired = true; });

  // ABS_VALUE_RANGE = 10, so distance of 9 should match
  clickAnalog(btns, BTN_A_VALUE + (ABS_VALUE_RANGE - 1));
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, value_at_range_boundary_does_not_trigger) {
  resetMatchVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setClickHandler([](Button2& b) { btnA_fired = true; });

  // Distance exactly equal to ABS_VALUE_RANGE: abs(x - value) < ABS_VALUE_RANGE is false
  clickAnalog(btns, BTN_A_VALUE + ABS_VALUE_RANGE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertFalse(btnA_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, unregistered_value_fires_no_button) {
  resetMatchVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setClickHandler([](Button2& b) { btnA_fired = true; });
  btns.add(BTN_B_VALUE, "B").setClickHandler([](Button2& b) { btnB_fired = true; });

  // 300 is far from both BTN_A_VALUE (100) and BTN_B_VALUE (500)
  clickAnalog(btns, 300);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertFalse(btnA_fired);
  assertFalse(btnB_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, correct_button_selected_among_multiple) {
  resetMatchVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setClickHandler([](Button2& b) { btnA_fired = true; });
  btns.add(BTN_B_VALUE, "B").setClickHandler([](Button2& b) { btnB_fired = true; });

  clickAnalog(btns, BTN_B_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertFalse(btnA_fired);
  assertTrue(btnB_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, zero_reading_does_not_trigger) {
  resetMatchVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setClickHandler([](Button2& b) { btnA_fired = true; });

  // A reading of 0 means "no button pressed" — should never fire a click
  simulatedAnalogValue = 0;
  for (int i = 0; i < 20; i++) {
    btns.loop();
    delay(5);
  }
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertFalse(btnA_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, constructor_tolerance_widens_window) {
  resetMatchVars();
  // Use a wider tolerance of 20
  AnalogButtons btns(TEST_PIN, false, 20);
  btns.setAnalogReadFunction(mockAnalogRead);
  btns.add(BTN_A_VALUE, "A").setClickHandler([](Button2& b) { btnA_fired = true; });

  // Distance 15 is within tolerance=20 but outside default ABS_VALUE_RANGE=10
  clickAnalog(btns, BTN_A_VALUE + 15);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, per_button_tolerance_overrides_default) {
  resetMatchVars();
  AnalogButtons btns = createTestButtons();  // default tolerance = ABS_VALUE_RANGE (10)
  // Give button A a wider tolerance of 25
  btns.add(BTN_A_VALUE, "A", 25).setClickHandler([](Button2& b) { btnA_fired = true; });
  btns.add(BTN_B_VALUE, "B").setClickHandler([](Button2& b) { btnB_fired = true; });

  // Distance 20: within A's tolerance (25) but not B's default (10)
  clickAnalog(btns, BTN_A_VALUE + 20);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
  assertFalse(btnB_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, per_button_tolerance_zero_inherits_default) {
  resetMatchVars();
  AnalogButtons btns = createTestButtons();  // default tolerance = ABS_VALUE_RANGE (10)
  // tolerance=0 means inherit default (10)
  btns.add(BTN_A_VALUE, "A", 0).setClickHandler([](Button2& b) { btnA_fired = true; });

  // Distance 9 is within the default ABS_VALUE_RANGE=10
  clickAnalog(btns, BTN_A_VALUE + 9);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
}

/////////////////////////////////////////////////////////////////

void setup() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
  delay(100);
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {}
  Serial.println(F("\n\nAnalogButtons Matching Tests"));
}

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
