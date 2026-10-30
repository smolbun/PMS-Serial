#include <Arduino.h>
#include "PMSSerial.h"

// Select one backend at compile time:
// - Define PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL for SoftwareSerial on pins 13/12.
// - Define PMS_SERIAL_TEST_USE_SERIAL1 for boards wired to Serial1.
#if defined(PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL)
#include "SoftwareSerial.h"
// (For Arduino Uno R4) any pin can be used for TX, but only the following pins can be used for RX:
// D0, D1, D2, D3, D8, D14, D15, A1, A2, A3, A4, A5
SoftwareSerial pmSerial(13, 12);  // RX, TX
PMSSerial pms(pmSerial);
#elif defined(PMS_SERIAL_TEST_USE_SERIAL1)
PMSSerial pms(Serial1);
#endif

#if defined(PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL) || defined(PMS_SERIAL_TEST_USE_SERIAL1)
void setup() {
  Serial.begin(115200);

#if defined(PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL)
  pmSerial.begin(9600);
#elif defined(PMS_SERIAL_TEST_USE_SERIAL1)
  Serial1.begin(9600);
#endif

  delay(2000);
  Serial.println("Started!");
}

void loop() {
  pms.update();

  PMSSerial::Measurement measurement;
  if (pms.read(measurement)) {
    Serial.print("PM1 ");
    Serial.print(static_cast<int>(measurement.pm1_0));
    Serial.println("ug/m3");

    Serial.print("PM2.5 ");
    Serial.print(static_cast<int>(measurement.pm2_5));
    Serial.println("ug/m3");

    Serial.print("PM10 ");
    Serial.print(static_cast<int>(measurement.pm10));
    Serial.println("ug/m3\n");
  }
}
#else
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Define PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL or PMS_SERIAL_TEST_USE_SERIAL1.");
}

void loop() {}
#endif
