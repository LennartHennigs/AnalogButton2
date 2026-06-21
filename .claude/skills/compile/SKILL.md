---
name: compile
description: Compile AnalogButtons examples across Arduino platforms using arduino-cli and PlatformIO. Pass a platform name (esp8266, esp32, nano, all) or leave blank to compile all.
---

Compile AnalogButtons examples to verify they build cleanly across target platforms.

Parse $ARGUMENTS:
- If a specific platform is named (esp8266, esp32, nano), run `pio run -e <matching env>` for that platform (environments: `Wemos_D1_Mini`, `M5_ESP32`, `Arduino_Nano`)
- If "examples" or nothing is given, run `cd test && ./compile_examples.sh` to compile all examples across all platforms
- If a specific example name is given, find the matching `.ino` file under `examples/` and compile it with `arduino-cli compile` for all three cores:
  - `esp8266:esp8266:d1_mini`
  - `esp32:esp32:esp32dev`
  - `arduino:avr:nano`

Note: The compile script **must be run from the `test/` directory** — it resolves `../examples` at load time.

Report any compiler errors clearly, quoting the relevant error lines. If compilation is clean, confirm which platforms and examples passed.
