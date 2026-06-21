/* ----------------------------------------------------- */
#include "AnalogButton2.h"
/* ----------------------------------------------------- */

// On AVR, std::function and capturing lambdas are unavailable.
// Ten static wrapper functions read the shared _g_states array by index.
// LIMITATION: Only one AnalogButton2 instance is supported on AVR.
// A second instance overwrites _g_states and breaks the first. Use
// ESP8266 or ESP32 (which support std::function) for multiple instances.
#ifndef BUTTON2_HAS_STD_FUNCTION
#warning "AnalogButton2: AVR target detected — only one AnalogButton2 instance is supported per sketch."
static_assert(ABS_MAX_BUTTONS <= 10, "AVR trampoline table has 10 entries — add matching _sfN functions before raising ABS_MAX_BUTTONS above 10.");
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

AnalogButton2::AnalogButton2(byte pin, bool show_unknown, uint16_t tolerance, byte maxButtons) {
  this->pin = pin;
  this->show_unknown = show_unknown;
  this->default_tolerance = tolerance;
#ifndef BUTTON2_HAS_STD_FUNCTION
  if (maxButtons > 10) maxButtons = 10;
#endif
  this->max_buttons = maxButtons;
  buttons    = new Button2[max_buttons];
  values     = new uint16_t[max_buttons];
  tolerances = new uint16_t[max_buttons];
  ids        = new String[max_buttons];
  states     = new uint8_t[max_buttons];
  for (byte i = 0; i < max_buttons; i++) states[i] = HIGH;
#ifndef BUTTON2_HAS_STD_FUNCTION
  _g_states = states;
#endif
}

AnalogButton2::~AnalogButton2() {
  delete[] buttons;
  delete[] values;
  delete[] tolerances;
  delete[] ids;
  delete[] states;
}

AnalogButton2::AnalogButton2(AnalogButton2&& other) noexcept
  : pin(other.pin), show_unknown(other.show_unknown), default_tolerance(other.default_tolerance),
    buttons(other.buttons), values(other.values), tolerances(other.tolerances),
    ids(other.ids), states(other.states), max_buttons(other.max_buttons),
    btn_count(other.btn_count), analog_read_fn(other.analog_read_fn),
    global_changed_fn(other.global_changed_fn),
    global_pressed_fn(other.global_pressed_fn),
    global_released_fn(other.global_released_fn),
    global_tap_fn(other.global_tap_fn),
    global_click_fn(other.global_click_fn),
    global_dbl_click_fn(other.global_dbl_click_fn),
    global_trpl_click_fn(other.global_trpl_click_fn),
    global_long_fn(other.global_long_fn),
    global_long_det_fn(other.global_long_det_fn)
{
  other.buttons    = nullptr;
  other.values     = nullptr;
  other.tolerances = nullptr;
  other.ids        = nullptr;
  other.states     = nullptr;
  other.btn_count  = 0;
  other.max_buttons = 0;
#ifndef BUTTON2_HAS_STD_FUNCTION
  _g_states = states;
#endif
}

/* ----------------------------------------------------- */

Button2* AnalogButton2::add(uint16_t value, String id, uint16_t tolerance) {
  if (btn_count >= max_buttons) return nullptr;

  byte i = btn_count++;
  values[i]     = value;
  tolerances[i] = (tolerance == ABS_INHERIT_TOLERANCE) ? default_tolerance : tolerance;
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

  // Apply any global handlers registered before this add() call.
  if (global_changed_fn)    buttons[i].setChangedHandler(global_changed_fn);
  if (global_pressed_fn)    buttons[i].setPressedHandler(global_pressed_fn);
  if (global_released_fn)   buttons[i].setReleasedHandler(global_released_fn);
  if (global_tap_fn)        buttons[i].setTapHandler(global_tap_fn);
  if (global_click_fn)      buttons[i].setClickHandler(global_click_fn);
  if (global_dbl_click_fn)  buttons[i].setDoubleClickHandler(global_dbl_click_fn);
  if (global_trpl_click_fn) buttons[i].setTripleClickHandler(global_trpl_click_fn);
  if (global_long_fn)       buttons[i].setLongClickHandler(global_long_fn);
  if (global_long_det_fn)   buttons[i].setLongClickDetectedHandler(global_long_det_fn);

  return &buttons[i];
}

/* ----------------------------------------------------- */

String AnalogButton2::getId(Button2& btn) {
  void* ctx = btn.getContext();
  return ctx ? *(String*)ctx : String();
}

/* ----------------------------------------------------- */

void AnalogButton2::reset() {
  for (byte i = 0; i < btn_count; i++) {
    buttons[i].reset();
    buttons[i].setDebounceTime(BTN_DEBOUNCE_MS);
    buttons[i].setLongClickTime(BTN_LONGCLICK_MS);
    buttons[i].setDoubleClickTime(BTN_DOUBLECLICK_MS);
    states[i] = HIGH;
  }
  btn_count = 0;
}

byte AnalogButton2::getCount() const { return btn_count; }
bool AnalogButton2::isFull()  const  { return btn_count >= max_buttons; }

/* ----------------------------------------------------- */

void AnalogButton2::setAnalogReadFunction(AnalogReadFunction f) {
  analog_read_fn = f;
}

/* ----------------------------------------------------- */

void AnalogButton2::setGlobalChangedHandler(CallbackFunction f) {
  global_changed_fn = f;
  for (byte i = 0; i < btn_count; i++) buttons[i].setChangedHandler(f);
}

void AnalogButton2::setGlobalPressedHandler(CallbackFunction f) {
  global_pressed_fn = f;
  for (byte i = 0; i < btn_count; i++) buttons[i].setPressedHandler(f);
}

void AnalogButton2::setGlobalReleasedHandler(CallbackFunction f) {
  global_released_fn = f;
  for (byte i = 0; i < btn_count; i++) buttons[i].setReleasedHandler(f);
}

void AnalogButton2::setGlobalTapHandler(CallbackFunction f) {
  global_tap_fn = f;
  for (byte i = 0; i < btn_count; i++) buttons[i].setTapHandler(f);
}

void AnalogButton2::setGlobalClickHandler(CallbackFunction f) {
  global_click_fn = f;
  for (byte i = 0; i < btn_count; i++) buttons[i].setClickHandler(f);
}

void AnalogButton2::setGlobalDoubleClickHandler(CallbackFunction f) {
  global_dbl_click_fn = f;
  for (byte i = 0; i < btn_count; i++) buttons[i].setDoubleClickHandler(f);
}

void AnalogButton2::setGlobalTripleClickHandler(CallbackFunction f) {
  global_trpl_click_fn = f;
  for (byte i = 0; i < btn_count; i++) buttons[i].setTripleClickHandler(f);
}

void AnalogButton2::setGlobalLongClickHandler(CallbackFunction f) {
  global_long_fn = f;
  for (byte i = 0; i < btn_count; i++) buttons[i].setLongClickHandler(f);
}

void AnalogButton2::setGlobalLongClickDetectedHandler(CallbackFunction f) {
  global_long_det_fn = f;
  for (byte i = 0; i < btn_count; i++) buttons[i].setLongClickDetectedHandler(f);
}

/* ----------------------------------------------------- */

void AnalogButton2::loop() {
  uint16_t reading = analog_read_fn ? analog_read_fn(pin) : (uint16_t)analogRead(pin);
  bool found = false;

  for (byte i = 0; i < btn_count; i++) states[i] = HIGH;

  // reading == 0 is the idle/unpressed state in a resistor-ladder circuit;
  // skip matching so a button registered near 0 is never spuriously fired.
  if (reading > 0) {
    for (byte i = 0; i < btn_count; i++) {
      if (abs((int32_t)reading - (int32_t)values[i]) <= tolerances[i]) {
        states[i] = LOW;
        found = true;
        break;
      }
    }

    if (show_unknown && !found) {
      Serial.print("unknown reading: ");
      Serial.println(reading);
    }
  }

  for (byte i = 0; i < btn_count; i++) {
    buttons[i].loop();
  }
}

/* ----------------------------------------------------- */
