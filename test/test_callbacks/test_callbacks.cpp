/////////////////////////////////////////////////////////////////
/*
  Callback tests for AnalogButtons library.
  Tests press/release/click handlers including Button2's richer events
  (double-click, long-click) working through the AnalogButtons wrapper.
*/
/////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <AUnitVerbose.h>
#include "../shared/test_helpers.h"

using namespace aunit;

/////////////////////////////////////////////////////////////////

#define SERIAL_SPEED 115200

/////////////////////////////////////////////////////////////////

static bool cb_pressed  = false;
static bool cb_released = false;
static bool cb_click    = false;
static bool cb_double   = false;
static bool cb_long     = false;
static int  cb_count    = 0;

void resetCbVars() {
  cb_pressed  = false;
  cb_released = false;
  cb_click    = false;
  cb_double   = false;
  cb_long     = false;
  cb_count    = 0;
  simulatedAnalogValue = 0;
}

/////////////////////////////////////////////////////////////////

test(callbacks, click_handler_fires) {
  resetCbVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setClickHandler([](Button2& b) { cb_click = true; });

  clickAnalog(btns, BTN_A_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(cb_click);
}

/////////////////////////////////////////////////////////////////

test(callbacks, pressed_handler_fires) {
  resetCbVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setPressedHandler([](Button2& b) { cb_pressed = true; });

  pressAnalog(btns, BTN_A_VALUE, DEBOUNCE_MS);

  assertTrue(cb_pressed);
  releaseAnalog(btns);
}

/////////////////////////////////////////////////////////////////

test(callbacks, released_handler_fires) {
  resetCbVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setReleasedHandler([](Button2& b) { cb_released = true; });

  clickAnalog(btns, BTN_A_VALUE);

  assertTrue(cb_released);
}

/////////////////////////////////////////////////////////////////

test(callbacks, global_click_handler_fires_for_all_buttons) {
  resetCbVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A");
  btns.add(BTN_B_VALUE, "B");
  btns.setGlobalClickHandler([](Button2& b) { cb_count++; });

  clickAnalog(btns, BTN_A_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();
  clickAnalog(btns, BTN_B_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertEqual(cb_count, 2);
}

/////////////////////////////////////////////////////////////////

test(callbacks, global_pressed_handler_fires_for_all_buttons) {
  resetCbVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A");
  btns.add(BTN_B_VALUE, "B");
  btns.setGlobalPressedHandler([](Button2& b) { cb_count++; });

  pressAnalog(btns, BTN_A_VALUE, DEBOUNCE_MS);
  releaseAnalog(btns);
  pressAnalog(btns, BTN_B_VALUE, DEBOUNCE_MS);
  releaseAnalog(btns);

  assertEqual(cb_count, 2);
}

/////////////////////////////////////////////////////////////////

test(callbacks, global_released_handler_fires_for_all_buttons) {
  resetCbVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A");
  btns.add(BTN_B_VALUE, "B");
  btns.setGlobalReleasedHandler([](Button2& b) { cb_count++; });

  clickAnalog(btns, BTN_A_VALUE);
  clickAnalog(btns, BTN_B_VALUE);

  assertEqual(cb_count, 2);
}

/////////////////////////////////////////////////////////////////

test(callbacks, button2_double_click_works) {
  resetCbVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setDoubleClickHandler([](Button2& b) { cb_double = true; });

  clickAnalog(btns, BTN_A_VALUE);
  clickAnalog(btns, BTN_A_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(cb_double);
}

/////////////////////////////////////////////////////////////////

test(callbacks, button2_long_click_works) {
  resetCbVars();
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "A").setLongClickHandler([](Button2& b) { cb_long = true; });

  pressAnalog(btns, BTN_A_VALUE, BTN_LONGCLICK_MS + 20);
  releaseAnalog(btns);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertTrue(cb_long);
}

/////////////////////////////////////////////////////////////////

test(callbacks, callback_receives_correct_button_reference) {
  resetCbVars();
  String captured_id = "";
  AnalogButtons btns = createTestButtons();
  btns.add(BTN_A_VALUE, "ALPHA").setClickHandler([&btns, &captured_id](Button2& b) {
    captured_id = btns.getId(b);
  });

  clickAnalog(btns, BTN_A_VALUE);
  delay(BTN_DOUBLECLICK_MS);
  btns.loop();

  assertEqual(captured_id, String("ALPHA"));
}

/////////////////////////////////////////////////////////////////

void setup() {
  TestRunner::setVerbosity(Verbosity::kDefault);
  TestRunner::list();
  delay(100);
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {}
  Serial.println(F("\n\nAnalogButtons Callback Tests"));
}

void loop() {
  aunit::TestRunner::run();
}

/////////////////////////////////////////////////////////////////
