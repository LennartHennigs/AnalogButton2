/////////////////////////////////////////////////////////////////
/*
  Value-matching tests for AnalogButton2 library.
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
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A")->setClickHandler([](Button2& b) { btnA_fired = true; });
  btns.add(BTN_B_VALUE, "B")->setClickHandler([](Button2& b) { btnB_fired = true; });

  clickAnalog(btns, BTN_A_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
  assertFalse(btnB_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, value_within_range_triggers) {
  resetMatchVars();
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A")->setClickHandler([](Button2& b) { btnA_fired = true; });

  // ABS_VALUE_RANGE = 10, so distance of 9 should match
  clickAnalog(btns, BTN_A_VALUE + (ABS_VALUE_RANGE - 1));
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, value_at_range_boundary_triggers) {
  resetMatchVars();
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A")->setClickHandler([](Button2& b) { btnA_fired = true; });

  // Distance exactly equal to ABS_VALUE_RANGE: abs(x - value) <= ABS_VALUE_RANGE matches
  clickAnalog(btns, BTN_A_VALUE + ABS_VALUE_RANGE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, value_beyond_range_does_not_trigger) {
  resetMatchVars();
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A")->setClickHandler([](Button2& b) { btnA_fired = true; });

  // Distance one beyond tolerance: abs(x - value) > ABS_VALUE_RANGE, no match
  clickAnalog(btns, BTN_A_VALUE + ABS_VALUE_RANGE + 1);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertFalse(btnA_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, unregistered_value_fires_no_button) {
  resetMatchVars();
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A")->setClickHandler([](Button2& b) { btnA_fired = true; });
  btns.add(BTN_B_VALUE, "B")->setClickHandler([](Button2& b) { btnB_fired = true; });

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
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A")->setClickHandler([](Button2& b) { btnA_fired = true; });
  btns.add(BTN_B_VALUE, "B")->setClickHandler([](Button2& b) { btnB_fired = true; });

  clickAnalog(btns, BTN_B_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertFalse(btnA_fired);
  assertTrue(btnB_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, zero_reading_does_not_trigger) {
  resetMatchVars();
  AnalogButton2 btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A")->setClickHandler([](Button2& b) { btnA_fired = true; });

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
  AnalogButton2 btns(TEST_PIN, false, 20);
  btns.setAnalogReadFunction(mockAnalogRead);
  btns.add(BTN_A_VALUE, "A")->setClickHandler([](Button2& b) { btnA_fired = true; });

  // Distance 15 is within tolerance=20 but outside default ABS_VALUE_RANGE=10
  clickAnalog(btns, BTN_A_VALUE + 15);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, per_button_tolerance_overrides_default) {
  resetMatchVars();
  AnalogButton2 btns = createTestButtons();  // default tolerance = ABS_VALUE_RANGE (10)
  // Give button A a wider tolerance of 25
  btns.add(BTN_A_VALUE, "A", 25)->setClickHandler([](Button2& b) { btnA_fired = true; });
  btns.add(BTN_B_VALUE, "B")->setClickHandler([](Button2& b) { btnB_fired = true; });

  // Distance 20: within A's tolerance (25) but not B's default (10)
  clickAnalog(btns, BTN_A_VALUE + 20);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
  assertFalse(btnB_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, omitted_tolerance_inherits_default) {
  resetMatchVars();
  AnalogButton2 btns = createTestButtons();  // default tolerance = ABS_VALUE_RANGE (10)
  // No tolerance argument → inherits default (ABS_INHERIT_TOLERANCE sentinel)
  btns.add(BTN_A_VALUE, "A")->setClickHandler([](Button2& b) { btnA_fired = true; });

  // Distance 9 is within the default ABS_VALUE_RANGE=10
  clickAnalog(btns, BTN_A_VALUE + 9);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(btnA_fired);
}

/////////////////////////////////////////////////////////////////

test(test_matching, per_button_tolerance_zero_means_exact_match) {
  resetMatchVars();
  AnalogButton2 btns = createTestButtons();
  // tolerance=0 means exact match only
  btns.add(BTN_A_VALUE, "A", 0)->setClickHandler([](Button2& b) { btnA_fired = true; });

  // Exact value fires
  clickAnalog(btns, BTN_A_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();
  assertTrue(btnA_fired);

  // One count away does not fire
  resetMatchVars();
  AnalogButton2 btns2 = createTestButtons();
  btns2.add(BTN_A_VALUE, "A", 0)->setClickHandler([](Button2& b) { btnA_fired = true; });
  clickAnalog(btns2, BTN_A_VALUE + 1);
  delay(BTN_DOUBLECLICK_MS);
  btns2.loop();
  assertFalse(btnA_fired);
}

/////////////////////////////////////////////////////////////////

void setup() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
  delay(100);
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {}
  Serial.println(F("\n\nAnalogButton2 Matching Tests"));
}

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
