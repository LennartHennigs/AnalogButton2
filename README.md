AnalogButtons
=============

* Author: Lennart Hennigs (https://www.lennarthennigs.de)
* Copyright (C) 2017-2026 Lennart Hennigs.
* Released under the MIT license.

Arduino Library to read multiple buttons wired to a single analog pin.


Description
-----------

Many button keyboards (e.g. LCD shield button rows) connect several buttons to one analog pin, each producing a distinct voltage level. `AnalogButtons` polls that pin in your `loop()`, identifies which button was pressed by matching the reading to a registered value within a configurable tolerance (±10), and delegates all event handling to [Button2](https://github.com/LennartHennigs/Button2) — giving you press, release, click, double-click, long-press, and retriggerable long-press for free.

Up to 10 buttons can be registered per pin. Analog readings cover the full 0–1023 range returned by `analogRead()`.

**Requires** the [Button2](https://github.com/LennartHennigs/Button2) library.


Calibration
-----------

Not sure what value to use for a button? Construct `AnalogButtons` with `show_unknown = true`:

```cpp
AnalogButtons btns(A0, true);
```

Any unrecognised reading (non-zero) will be printed to `Serial`. Press each button and note the values, then register them with `add()`.


Usage
-----

```cpp
#include <Button2.h>
#include <AnalogButtons.h>

AnalogButtons btns(A0);

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

`add()` returns a `Button2&`, so you can chain any Button2 handler — `setDoubleClickHandler()`, `setLongClickHandler()`, `setLongClickDetectedHandler()`, etc. — directly on the returned reference.

You can also register a handler for all buttons at once:

```cpp
btns.setGlobalClickHandler(clickHandler);
```


API
---

| Method | Description |
|--------|-------------|
| `AnalogButtons(byte pin, bool show_unknown = false)` | Constructor. `show_unknown` prints raw readings for unregistered presses to Serial. |
| `Button2& add(uint16_t value, String id = "")` | Register a button at the given ADC value. Returns `Button2&` for chaining handlers. |
| `String getId(Button2& btn)` | Retrieve the string label for a button inside a callback. |
| `void setAnalogReadFunction(f)` | Inject a custom ADC reader (used by the test suite; replaces `analogRead`). |
| `void setGlobalClickHandler(f)` | Set the same click handler on all registered buttons. |
| `void setGlobalPressedHandler(f)` | Set the same pressed handler on all registered buttons. |
| `void setGlobalReleasedHandler(f)` | Set the same released handler on all registered buttons. |
| `void loop()` | Call this in your Arduino `loop()`. Reads the pin and drives Button2 state machines. |


Installation
------------

Open the Arduino IDE, choose **Sketch > Include Library > Manage Libraries…** and search for "AnalogButtons". Or download the ZIP from [GitHub](https://github.com/LennartHennigs/AnalogButton2) and choose **Sketch > Include Library > Add .ZIP Library…**.

You must also have the [Button2](https://github.com/LennartHennigs/Button2) library installed.


License
-------

MIT License

Copyright (c) 2017-2026 Lennart Hennigs

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
