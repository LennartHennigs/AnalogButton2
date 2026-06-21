---
name: test
description: Run AnalogButtons test suites via PlatformIO. Optionally pass a suite name (basics, matching, callbacks) or platform (esp8266, esp32). Defaults to all suites on ESP8266.
---

Run the AnalogButtons test suite using PlatformIO + EpoxyDuino (no hardware required).

Parse $ARGUMENTS:
- If a specific suite is named (basics, matching, callbacks), run `pio test -e test_<suite>`
- If a platform is named (esp8266, esp32), run `pio test -e epoxy-<platform>`
- If "all" or nothing is given, run `pio test -e epoxy-esp8266 -e epoxy-esp32`

Show the filtered output (`grep -E '(PASSED|FAILED|ERRORED|summary)'`) and report how many suites passed vs failed.

If any suite fails, show the full verbose output for the failing suite so the failure is easy to diagnose.
