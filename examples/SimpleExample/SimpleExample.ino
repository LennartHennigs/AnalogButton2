/* ----------------------------------------------------- */

#include <Button2.h>
#include <AnalogButton2.h>

/* ----------------------------------------------------- */

AnalogButtons btns(A0, true);

/* ----------------------------------------------------- */

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(100);
  Serial.println("\n\nAnalog Buttons Test\n");

  btns.add( 18, "LEFT").setClickHandler(clickHandler);
  btns.add( 42, "CENTER").setClickHandler(clickHandler);
  btns.add( 59, "RIGHT").setClickHandler(clickHandler);
  btns.add( 88, "BOTTOM").setClickHandler(clickHandler);
  btns.add(182, "TOP").setClickHandler(clickHandler);
}

/* ----------------------------------------------------- */

void loop() {
  btns.loop();
}

/* ----------------------------------------------------- */

void clickHandler(Button2& btn) {
  Serial.print("Button: ");
  Serial.print(btns.getId(btn));
  Serial.print(" (");
  Serial.print(btn.wasPressedFor());
  Serial.println("ms)");
}

/* ----------------------------------------------------- */
