/* ----------------------------------------------------- */
/*
  AnalogButtons.h - Arduino Library to read multiple buttons on a single analog pin.
  Created by Lennart Hennigs, October 28, 2017.
*/
/* ----------------------------------------------------- */

#pragma once

#include <Arduino.h>
#include <Button2.h>

/* ----------------------------------------------------- */

#define ABS_VALUE_RANGE      10
#define ABS_MAX_BUTTONS      10

/* ----------------------------------------------------- */

class AnalogButtons {
  private:
    byte pin;
    bool show_unknown = false;
    uint16_t default_tolerance;

    Button2  buttons[ABS_MAX_BUTTONS];
    uint16_t values[ABS_MAX_BUTTONS];
    uint16_t tolerances[ABS_MAX_BUTTONS];
    String   ids[ABS_MAX_BUTTONS];
    uint8_t  states[ABS_MAX_BUTTONS];
    byte     btn_count = 0;

#ifdef BUTTON2_HAS_STD_FUNCTION
    typedef std::function<uint16_t(byte)> AnalogReadFunction;
    typedef std::function<void(Button2&)> CallbackFunction;
#else
    typedef uint16_t (*AnalogReadFunction)(byte);
    typedef void (*CallbackFunction)(Button2&);
#endif
    AnalogReadFunction analog_read_fn = BUTTON2_NULL;

  public:
    AnalogButtons(byte pin, bool show_unknown = false, uint16_t tolerance = ABS_VALUE_RANGE);

    // tolerance = 0 inherits the constructor default
    Button2& add(uint16_t value, String id = "", uint16_t tolerance = 0);

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
