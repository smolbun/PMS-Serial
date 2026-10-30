#include <Arduino.h>

#if __has_include("PMSSerial.h")
#include "PMSSerial.h"
#elif __has_include("../src/PMSSerial.h")
#include "../src/PMSSerial.h"
#else
#error "PMSSerial.h not found. Install/copy this repository as an Arduino library or keep this sketch inside the repository."
#endif

// Define PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL to enable sensor reading on pins 13/12.
#if defined(PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL)
#include "SoftwareSerial.h"
// (For Arduino Uno R4) any pin can be used for TX, but only the following pins can be used for RX:
// D0, D1, D2, D3, D8, D14, D15, A1, A2, A3, A4, A5
SoftwareSerial pmSerial(13, 12);  // RX, TX
PMSSerial pms(pmSerial);
#endif

#if defined(PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL)
void setup() {
  Serial.begin(115200);
  pmSerial.begin(9600);

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
  Serial.println("Define PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL to run the SoftwareSerial test sketch.");
}

void loop() {}
#endif
