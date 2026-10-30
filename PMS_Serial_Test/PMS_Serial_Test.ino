#include <Arduino.h>
#include <PMSSerial.h>

// Default path: use Serial1 (works on boards with a hardware UART exposed as Serial1).
// Optional path: define PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL before this sketch is compiled
// to use SoftwareSerial with the original Uno R4 example pins.
#if defined(PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL)
#include <SoftwareSerial.h>
// (For Arduino Uno R4) any pin can be used for TX, but only the following pins can be used for RX:
// D0, D1, D2, D3, D8, D14, D15, A1, A2, A3, A4, A5
SoftwareSerial pmSerial(13, 12);  // RX, TX
Stream& pmStream = pmSerial;
#else
#if !defined(HAVE_HWSERIAL1)
#error "This example defaults to Serial1. Define PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL to use SoftwareSerial instead."
#endif
Stream& pmStream = Serial1;
#endif

PMSSerial pms(pmStream);

void setup() {
  Serial.begin(115200);

#if defined(PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL)
  pmSerial.begin(9600);
#else
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
