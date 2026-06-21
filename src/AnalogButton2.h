/* ----------------------------------------------------- */
/*
  AnalogButton2.h - Arduino Library to read multiple buttons on a single analog pin.
  Created by Lennart Hennigs, October 28, 2017.
*/
/* ----------------------------------------------------- */

#pragma once

#include <Arduino.h>
#include <Button2.h>

/* ----------------------------------------------------- */

#define ABS_VALUE_RANGE      10
#define ABS_MAX_BUTTONS      10
#define ABS_INHERIT_TOLERANCE UINT16_MAX

/* ----------------------------------------------------- */

class AnalogButton2 {
  public:
#ifdef BUTTON2_HAS_STD_FUNCTION
    typedef std::function<uint16_t(byte)> AnalogReadFunction;
    typedef std::function<void(Button2&)> CallbackFunction;
#else
    typedef uint16_t (*AnalogReadFunction)(byte);
    typedef void (*CallbackFunction)(Button2&);
#endif

  private:
    byte pin;
    bool show_unknown = false;
    uint16_t default_tolerance;

    Button2*  buttons    = nullptr;
    uint16_t* values     = nullptr;
    uint16_t* tolerances = nullptr;
    String*   ids        = nullptr;
    uint8_t*  states     = nullptr;
    byte      max_buttons;
    byte      btn_count = 0;

    AnalogReadFunction analog_read_fn     = BUTTON2_NULL;

    CallbackFunction global_changed_fn    = BUTTON2_NULL;
    CallbackFunction global_pressed_fn    = BUTTON2_NULL;
    CallbackFunction global_released_fn   = BUTTON2_NULL;
    CallbackFunction global_tap_fn        = BUTTON2_NULL;
    CallbackFunction global_click_fn      = BUTTON2_NULL;
    CallbackFunction global_dbl_click_fn  = BUTTON2_NULL;
    CallbackFunction global_trpl_click_fn = BUTTON2_NULL;
    CallbackFunction global_long_fn       = BUTTON2_NULL;
    CallbackFunction global_long_det_fn   = BUTTON2_NULL;

  public:
    AnalogButton2(byte pin, bool show_unknown = false, uint16_t tolerance = ABS_VALUE_RANGE, byte maxButtons = ABS_MAX_BUTTONS);
    ~AnalogButton2();
    AnalogButton2(AnalogButton2&& other) noexcept;
    AnalogButton2(const AnalogButton2&) = delete;
    AnalogButton2& operator=(const AnalogButton2&) = delete;
    AnalogButton2& operator=(AnalogButton2&&) = delete;

    // Returns nullptr when maxButtons is exceeded. tolerance=0 requires exact match;
    // omit or pass ABS_INHERIT_TOLERANCE to use the instance default.
    Button2* add(uint16_t value, String id = "", uint16_t tolerance = ABS_INHERIT_TOLERANCE);

    String getId(Button2& btn);

    void reset();
    byte getCount() const;
    bool isFull() const;

    void setAnalogReadFunction(AnalogReadFunction f);

    void setGlobalChangedHandler(CallbackFunction f);
    void setGlobalPressedHandler(CallbackFunction f);
    void setGlobalReleasedHandler(CallbackFunction f);
    void setGlobalTapHandler(CallbackFunction f);
    void setGlobalClickHandler(CallbackFunction f);
    void setGlobalDoubleClickHandler(CallbackFunction f);
    void setGlobalTripleClickHandler(CallbackFunction f);
    void setGlobalLongClickHandler(CallbackFunction f);
    void setGlobalLongClickDetectedHandler(CallbackFunction f);

    void loop();
};

/* ----------------------------------------------------- */
