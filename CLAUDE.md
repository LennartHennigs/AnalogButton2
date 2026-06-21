# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino library for reading multiple buttons wired to a single analog pin. Each button produces a distinct analog voltage level; the library maps those readings to named button objects and fires callbacks on press, release, and click events.

## Architecture

The library has one class: **`AnalogButton2`** (`src/AnalogButton2.h/.cpp`). It depends on the [Button2](https://github.com/LennartHennigs/Button2) library for all button event handling.

Internally, `AnalogButton2` owns a fixed-size array of up to `ABS_MAX_BUTTONS` (10) real `Button2` instances. Each instance is created with `BTN_VIRTUAL_PIN` and a lambda state function that returns the button's current `states[i]` byte (`LOW` = pressed, `HIGH` = released). `AnalogButton2::loop()` reads the analog pin once per call, updates `states[]` by matching the reading to each button's registered value within `ABS_VALUE_RANGE` (±10), then calls `button.loop()` on every instance — which drives Button2's full state machine (debounce, click/double-click/long-press detection, callbacks).

`add()` returns a `Button2&`, giving callers direct access to the full Button2 API per button. String labels are stored in a parallel `ids[]` array and attached to each Button2 instance via `setContext()`; `getId(Button2&)` retrieves them in callbacks.

Key design decisions:
- All event logic (debounce, timing, multi-click) lives in Button2 — this library only handles analog multiplexing.
- In `add()`, `setButtonStateFunction` must be called **before** `begin()` so that Button2's initial `_getState()` call inside `begin()` reads `HIGH` (released) via our function rather than `digitalRead(BTN_VIRTUAL_PIN)`, which returns `LOW` (pressed) in EpoxyDuino and causes spurious press/release events.
- `show_unknown` mode prints unrecognised readings to `Serial` — useful for calibrating button values.
- Button values and analog readings are `uint16_t`, covering the full 0–1023 range returned by `analogRead()`.
- On AVR (no `std::function`), only one `AnalogButton2` instance is supported — a second instance overwrites the shared `_g_states` array and breaks the first. ESP8266/ESP32 support multiple instances via capturing lambdas.
- `setGlobalClickHandler()`, `setGlobalPressedHandler()`, `setGlobalReleasedHandler()` — apply a single callback to all registered buttons at once.

## Development

### Unit tests (no hardware required)

Tests use [EpoxyDuino](https://github.com/bxparks/EpoxyDuino) + [AUnit](https://github.com/bxparks/AUnit) and run natively via PlatformIO:

```bash
# Run all suites
pio test -e epoxy-esp8266 -v

# Run all suites simulating ESP32 target
pio test -e epoxy-esp32 -v

# Run one suite
pio test -e test_basics -v
pio test -e test_matching -v
pio test -e test_callbacks -v
```

The test helper injects a mock `analogRead` via `setAnalogReadFunction()` — this hook is also what makes the real library testable without hardware. See `test/shared/test_helpers.h`.

### Example compilation check

```bash
cd test
./compile_examples.sh              # all platforms via PlatformIO
./compile_examples.sh esp32        # ESP32 only
./compile_examples.sh --tool=arduino-cli  # use arduino-cli instead
```

### Upload to hardware

```bash
# Compile and upload (adjust board/port as needed)
arduino-cli compile --fqbn esp8266:esp8266:d1_mini examples/SimpleExample/SimpleExample.ino
arduino-cli upload -p /dev/cu.usbmodem* --fqbn esp8266:esp8266:d1_mini examples/SimpleExample/SimpleExample.ino
arduino-cli monitor -p /dev/cu.usbmodem* --config baudrate=9600
```

When calibrating button values, construct `AnalogButton2` with `show_unknown = true` — it prints raw ADC readings for any unrecognised presses to Serial.

## Repository

GitHub: https://github.com/LennartHennigs/AnalogButton2

### Publishing a release

- PlatformIO: `pio pkg publish --no-interactive`
- Arduino Library Manager: open an issue in `arduino/library-registry` with the repo URL — the bot validates and merges automatically.

### compile_examples.sh notes

- **Must be run from `test/`** — the script resolves `../examples` at load time; running it from the repo root (`bash test/compile_examples.sh`) finds no examples and exits with "No tests were executed!".
- ESP32 test board is `esp32dev` (not `m5stack_core2`) — M5Stack Core2 uses `G0`/`G1` GPIO aliases and does not define `A0`.
- `espressif32` platform is pinned to `^6.9.0` in the generated temp ini — Arduino ESP32 3.x (espressif32 ≥ 7.x) has a `y0()` Bessel-function conflict that breaks `Tone.cpp`.
- If ESP builds fail with `No module named 'intelhex'`, the script auto-installs it into PlatformIO's Python on first run.
