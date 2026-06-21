AnalogButton2
=============

* Author: Lennart Hennigs (https://www.lennarthennigs.de)
* Copyright (C) 2017-2026 Lennart Hennigs.
* Released under the MIT license.

Arduino Library to read multiple buttons wired to a single analog pin.


Description
-----------

Many button keyboards (e.g. LCD shield button rows) connect several buttons to one analog pin, each producing a distinct voltage level. `AnalogButton2` polls that pin in your `loop()`, identifies which button was pressed by matching the reading to a registered value within a configurable tolerance (±10), and delegates all event handling to [Button2](https://github.com/LennartHennigs/Button2) — giving you press, release, click, double-click, long-press, and retriggerable long-press for free.

Up to 10 buttons can be registered per pin. Analog readings cover the full 0–1023 range returned by `analogRead()`.

**Requires** the [Button2](https://github.com/LennartHennigs/Button2) library.


Calibration
-----------

Not sure what value to use for a button? Construct `AnalogButton2` with `show_unknown = true`:

```cpp
AnalogButton2 btns(A0, true);
```

Any unrecognised reading (non-zero) will be printed to `Serial`. Press each button and note the values, then register them with `add()`.


Usage
-----

```cpp
#include <Button2.h>
#include <AnalogButton2.h>

AnalogButton2 btns(A0);

void setup() {
  Serial.begin(9600);

  btns.add( 18, "LEFT").setClickHandler(clickHandler);
  btns.add( 42, "CENTER").setClickHandler(clickHandler);
  btns.add( 59, "RIGHT").setClickHandler(clickHandler);
}

void loop() {
  btns.loop();
}

void clickHandler(Button2& btn) {
  Serial.print("Button: ");
  Serial.print(btns.getId(btn));
  Serial.print(" (");
  Serial.print(btn.wasPressedFor());
  Serial.println("ms)");
}
```

`add()` returns a `Button2&`, so you can chain any Button2 handler directly on the returned reference:

| Handler | Fires when… |
|---------|-------------|
| `setTapHandler(f)` | Any press — immediately on release, ignoring multi-click timing |
| `setClickHandler(f)` | Single click confirmed (slight delay to rule out double/triple) |
| `setDoubleClickHandler(f)` | Double click |
| `setTripleClickHandler(f)` | Triple click |
| `setLongClickDetectedHandler(f)` | Long press threshold reached (button still held) |
| `setLongClickHandler(f)` | Long press confirmed after release |
| `setPressedHandler(f)` | Button goes down |
| `setReleasedHandler(f)` | Button goes up |
| `setChangedHandler(f)` | Any state change |

> **Tip:** Use `setTapHandler()` or `setLongClickDetectedHandler()` when you need immediate feedback. `setClickHandler()` waits briefly to confirm no double-click follows.

For retriggerable long press, call `setLongClickDetectedRetriggerable(true)` on the returned `Button2&`; `getLongClickCount()` gives you the repeat count.

You can also register a handler for all buttons at once:

```cpp
btns.setGlobalClickHandler(clickHandler);
```

### Timeouts

Button2 defaults (in ms): debounce `50`, long-click `200`, double-click `300`. Override per button:

```cpp
btns.add(42, "CENTER")
    .setDoubleClickTime(400)
    .setLongClickTime(500)
    .setClickHandler(clickHandler);
```

### wasPressedFor()

Returns the duration of the **most recent individual press**, not cumulative. For a double-click (50 ms + 80 ms), `wasPressedFor()` returns `80`.


API
---

| Method | Description |
|--------|-------------|
| `AnalogButton2(byte pin, bool show_unknown = false, uint16_t tolerance = 10)` | Constructor. `tolerance` sets the default ADC match window (±value). |
| `Button2& add(uint16_t value, String id = "", uint16_t tolerance = 0)` | Register a button. `tolerance = 0` inherits the constructor default. Returns `Button2&` for chaining. |
| `String getId(Button2& btn)` | Retrieve the string label for a button inside a callback. |
| `void reset()` | Clear all registered buttons; the object can be re-used with new `add()` calls. |
| `byte getCount() const` | Number of currently registered buttons. |
| `bool isFull() const` | `true` when the 10-button limit is reached. |
| `void setAnalogReadFunction(f)` | Inject a custom ADC reader (used by the test suite; replaces `analogRead`). |
| `void setGlobalChangedHandler(f)` | Set the same changed handler on all registered buttons. |
| `void setGlobalPressedHandler(f)` | Set the same pressed handler on all registered buttons. |
| `void setGlobalReleasedHandler(f)` | Set the same released handler on all registered buttons. |
| `void setGlobalTapHandler(f)` | Set the same tap handler on all registered buttons. |
| `void setGlobalClickHandler(f)` | Set the same click handler on all registered buttons. |
| `void setGlobalDoubleClickHandler(f)` | Set the same double-click handler on all registered buttons. |
| `void setGlobalTripleClickHandler(f)` | Set the same triple-click handler on all registered buttons. |
| `void setGlobalLongClickHandler(f)` | Set the same long-click handler on all registered buttons. |
| `void setGlobalLongClickDetectedHandler(f)` | Set the same long-click-detected handler on all registered buttons. |
| `void loop()` | Call this in your Arduino `loop()`. Reads the pin and drives Button2 state machines. |


Installation
------------

Open the Arduino IDE, choose **Sketch > Include Library > Manage Libraries…** and search for "AnalogButton2". Or download the ZIP from [GitHub](https://github.com/LennartHennigs/AnalogButton2) and choose **Sketch > Include Library > Add .ZIP Library…**.

You must also have the [Button2](https://github.com/LennartHennigs/Button2) library installed.


License
-------

MIT License

Copyright (c) 2017-2026 Lennart Hennigs

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
