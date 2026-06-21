/* ----------------------------------------------------- */
#include "AnalogButton2.h"
/* ----------------------------------------------------- */

// On AVR, std::function and capturing lambdas are unavailable.
// Ten static wrapper functions read the shared _g_states array by index.
// LIMITATION: Only one AnalogButtons instance is supported on AVR.
// A second instance overwrites _g_states and breaks the first. Use
// ESP8266 or ESP32 (which support std::function) for multiple instances.
#ifndef BUTTON2_HAS_STD_FUNCTION
#warning "AnalogButtons: AVR target detected — only one AnalogButtons instance is supported per sketch."
static uint8_t* _g_states = nullptr;
static uint8_t _sf0() { return _g_states[0]; }
static uint8_t _sf1() { return _g_states[1]; }
static uint8_t _sf2() { return _g_states[2]; }
static uint8_t _sf3() { return _g_states[3]; }
static uint8_t _sf4() { return _g_states[4]; }
static uint8_t _sf5() { return _g_states[5]; }
static uint8_t _sf6() { return _g_states[6]; }
static uint8_t _sf7() { return _g_states[7]; }
static uint8_t _sf8() { return _g_states[8]; }
static uint8_t _sf9() { return _g_states[9]; }
typedef uint8_t (*_SlotFn)();
static _SlotFn _g_fns[ABS_MAX_BUTTONS] = {
  _sf0, _sf1, _sf2, _sf3, _sf4, _sf5, _sf6, _sf7, _sf8, _sf9
};
#endif

/* ----------------------------------------------------- */

AnalogButtons::AnalogButtons(byte pin, bool show_unknown, uint16_t tolerance) {
  this->pin = pin;
  this->show_unknown = show_unknown;
  this->default_tolerance = tolerance;
  memset(states, HIGH, sizeof(states));
#ifndef BUTTON2_HAS_STD_FUNCTION
  _g_states = states;
#endif
}

/* ----------------------------------------------------- */

Button2& AnalogButtons::add(uint16_t value, String id, uint16_t tolerance) {
  if (btn_count >= ABS_MAX_BUTTONS) return buttons[ABS_MAX_BUTTONS - 1];

  byte i = btn_count++;
  values[i]     = value;
  tolerances[i] = (tolerance == 0) ? default_tolerance : tolerance;
  ids[i]        = (id != "") ? id : String(value);
  states[i]     = HIGH;

  // State function must be set BEFORE begin() so begin()'s _getState() call
  // reads HIGH (released) via our function instead of digitalRead(BTN_VIRTUAL_PIN).
#ifdef BUTTON2_HAS_STD_FUNCTION
  uint8_t* sp = &states[i];
  buttons[i].setButtonStateFunction([sp]() -> uint8_t { return *sp; });
#else
  buttons[i].setButtonStateFunction(_g_fns[i]);
#endif

  buttons[i].begin(BTN_VIRTUAL_PIN);
  buttons[i].setContext((void*)&ids[i]);

  return buttons[i];
}

/* ----------------------------------------------------- */

String AnalogButtons::getId(Button2& btn) {
  return *(String*)btn.getContext();
}

/* ----------------------------------------------------- */

void AnalogButtons::reset() {
  for (byte i = 0; i < btn_count; i++) {
    buttons[i].reset();
    states[i] = HIGH;
  }
  btn_count = 0;
}

byte AnalogButtons::getCount() const { return btn_count; }
bool AnalogButtons::isFull()  const  { return btn_count >= ABS_MAX_BUTTONS; }

/* ----------------------------------------------------- */

void AnalogButtons::setAnalogReadFunction(AnalogReadFunction f) {
  analog_read_fn = f;
}

/* ----------------------------------------------------- */

void AnalogButtons::setGlobalChangedHandler(CallbackFunction f) {
  for (byte i = 0; i < btn_count; i++) buttons[i].setChangedHandler(f);
}

void AnalogButtons::setGlobalPressedHandler(CallbackFunction f) {
  for (byte i = 0; i < btn_count; i++) buttons[i].setPressedHandler(f);
}

void AnalogButtons::setGlobalReleasedHandler(CallbackFunction f) {
  for (byte i = 0; i < btn_count; i++) buttons[i].setReleasedHandler(f);
}

void AnalogButtons::setGlobalTapHandler(CallbackFunction f) {
  for (byte i = 0; i < btn_count; i++) buttons[i].setTapHandler(f);
}

void AnalogButtons::setGlobalClickHandler(CallbackFunction f) {
  for (byte i = 0; i < btn_count; i++) buttons[i].setClickHandler(f);
}

void AnalogButtons::setGlobalDoubleClickHandler(CallbackFunction f) {
  for (byte i = 0; i < btn_count; i++) buttons[i].setDoubleClickHandler(f);
}

void AnalogButtons::setGlobalTripleClickHandler(CallbackFunction f) {
  for (byte i = 0; i < btn_count; i++) buttons[i].setTripleClickHandler(f);
}

void AnalogButtons::setGlobalLongClickHandler(CallbackFunction f) {
  for (byte i = 0; i < btn_count; i++) buttons[i].setLongClickHandler(f);
}

void AnalogButtons::setGlobalLongClickDetectedHandler(CallbackFunction f) {
  for (byte i = 0; i < btn_count; i++) buttons[i].setLongClickDetectedHandler(f);
}

/* ----------------------------------------------------- */

void AnalogButtons::loop() {
  uint16_t reading = analog_read_fn ? analog_read_fn(pin) : (uint16_t)analogRead(pin);
  bool found = false;

  for (byte i = 0; i < btn_count; i++) {
    if (abs((int32_t)reading - (int32_t)values[i]) < tolerances[i]) {
      states[i] = LOW;
      found = true;
    } else {
      states[i] = HIGH;
    }
  }

  if (show_unknown && !found && reading > 0) {
    Serial.print("unknown reading: ");
    Serial.println(reading);
  }

  for (byte i = 0; i < btn_count; i++) {
    buttons[i].loop();
  }
}

/* ----------------------------------------------------- */
