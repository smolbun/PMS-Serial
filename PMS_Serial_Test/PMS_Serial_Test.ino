#include <Arduino.h>
#include <PMSSerial.h>

// This example is intentionally SoftwareSerial-based (original Uno R4 wiring).
// To use a hardware UART on other boards, keep this parser library and write a board-specific sketch.
#if defined(ARDUINO_ARCH_RENESAS) || defined(ARDUINO_ARCH_AVR)
#include <SoftwareSerial.h>
// (For Arduino Uno R4) any pin can be used for TX, but only the following pins can be used for RX:
// D0, D1, D2, D3, D8, D14, D15, A1, A2, A3, A4, A5
SoftwareSerial pmSerial(13, 12);  // RX, TX
PMSSerial pms(pmSerial);

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
  Serial.println("PMS_Serial_Test uses SoftwareSerial and is intended for Uno R4/AVR targets.");
  Serial.println("Use PMSSerial with your board's Stream (for example Serial1) in a custom sketch.");
}

void loop() {}
#endif
