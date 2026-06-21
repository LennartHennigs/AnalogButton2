/* ----------------------------------------------------- */
/*
  CalibrateExample - Use show_unknown to find button values.

  Wire your buttons to an analog pin, open the Serial Monitor
  at 9600 baud, and press each button one at a time.
  The raw ADC reading is printed for every unrecognised press.
  Note those values, then use them in your final sketch with add().
*/
/* ----------------------------------------------------- */

#include <Button2.h>
#include <AnalogButtons.h>

/* ----------------------------------------------------- */

// Pass true to enable show_unknown mode.
// Any analog reading that doesn't match a registered button
// is printed to Serial so you can record the value.
AnalogButtons btns(A0, true);

/* ----------------------------------------------------- */

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(100);
  Serial.println("\n\nAnalogButtons Calibration\n");
  Serial.println("Press each button and note the printed ADC value.");
  Serial.println("Use those values in add() in your final sketch.\n");

  // No buttons registered — every press prints an unknown reading.
  // Once you have the values, add them here and re-upload to verify:
  //   btns.add(VALUE_A, "A").setClickHandler(clickHandler);
  //   btns.add(VALUE_B, "B").setClickHandler(clickHandler);
}

/* ----------------------------------------------------- */

void loop() {
  btns.loop();
}

/* ----------------------------------------------------- */
