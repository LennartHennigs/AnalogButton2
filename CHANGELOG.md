# Changelog

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

## [1.0.0] - 2026-06-21

### Added

- `AnalogButtons(pin, show_unknown, tolerance)` — new `tolerance` constructor parameter sets the default ADC match window (replaces hard-coded `ABS_VALUE_RANGE`).
- `Button2& add(value, id, tolerance)` — new per-button `tolerance` parameter; `0` inherits the constructor default.
- `reset()` — clears all registered buttons so the object can be reused.
- `getCount() const` — returns the number of currently registered buttons.
- `isFull() const` — returns `true` when the 10-button limit is reached.
- `setGlobalChangedHandler(f)` — registers the same changed handler on all buttons.
- `setGlobalTapHandler(f)` — registers the same tap handler on all buttons.
- `setGlobalDoubleClickHandler(f)` — registers the same double-click handler on all buttons.
- `setGlobalTripleClickHandler(f)` — registers the same triple-click handler on all buttons.
- `setGlobalLongClickHandler(f)` — registers the same long-click handler on all buttons.
- `setGlobalLongClickDetectedHandler(f)` — registers the same long-click-detected handler on all buttons.
- `examples/CalibrateExample/` — new example showing the `show_unknown = true` calibration workflow.

### Internal

- AVR builds now emit a `#warning` at compile time noting the one-instance limit.
- Matching loop uses per-button `tolerances[i]` instead of the global `ABS_VALUE_RANGE` constant.
- Cast widened to `int32_t` in matching arithmetic to avoid overflow on boundary values.
- Test suites expanded: basics (+68 lines), callbacks (+143 lines), matching (+63 lines).

## [2.0.0] - 2026-06-20

### Changed

- **Breaking**: Replaced custom `AnalogButton` class with `Button2` instances backed by virtual pins (`BTN_VIRTUAL_PIN` + `setButtonStateFunction`). Users now receive a `Button2&` from `add()` and have access to the full Button2 event API (double-click, long-press, retriggerable long-press, etc.).
- **Breaking**: `AnalogButton.h` / `AnalogButton.cpp` deleted; `Button2` is now a required dependency.
- **Breaking**: `add()` signature changed from `add(AnalogButton)` to `Button2& add(uint16_t value, String id = "")`.
- **Breaking**: Callback signature changed from `void(AnalogButton&)` to `void(Button2&)`.
- **Breaking**: Button value and analog reading types changed from `byte` (0–255) to `uint16_t` (0–1023).
- `ABS_VALUE_RANGE` increased from 6 to 10 to better suit the full 0–1023 ADC range.
- `debounce_ms` constructor parameter removed; Button2 handles debouncing internally (50 ms default).
- `library.properties`: added `depends=Button2`, updated `sentence`, `paragraph`, and `category`.

### Added

- `getId(Button2& btn)` method to retrieve the string label for a button inside a callback.
- `setAnalogReadFunction(f)` hook to inject a custom ADC reader — used by the test suite to simulate readings without hardware.
- `setGlobalClickHandler()`, `setGlobalPressedHandler()`, `setGlobalReleasedHandler()` to register a single handler across all buttons at once.
- PlatformIO project (`platformio.ini`) with hardware upload environments (D1 Mini, M5Stack, Nano) and native EpoxyDuino test environments.
- Test suite (`test/`) with three suites covering basics, value matching, and callbacks (run via `pio test -e epoxy-esp8266`).
- Example compile script (`test/compile_examples.sh`) supporting PlatformIO and arduino-cli across three platforms.
- AVR support via static slot functions (10 pre-defined wrappers) since lambdas with captures are unavailable on AVR. One `AnalogButtons` instance per sketch on AVR; multiple instances work on ESP8266/ESP32.

## [1.0.0] - 2017-10-28

- Initial release
