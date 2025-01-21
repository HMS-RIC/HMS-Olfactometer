
#include "Olfactometer_header.h"

void setup() {

  Serial.begin(115200);
  while (!Serial) {
      delay(100);
  }

  setupMFC(1);
  setupMFC(2);
  InitializeValves();
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(1000);
  Serial.println("Turning Odor-1 On/Off");
  activateOdorValve(1);
  delay(500);
  deactivateOdorValve(1);
  delay(500);
  Serial.println("Turning Aux-1 On/Off");
  activateAuxValve(1);
  delay(500);
  deactivateAuxValve(1);

}
