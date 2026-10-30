# PMS-Serial

Reusable Arduino library for reading PMS particulate-matter sensors over a serial `Stream` (`HardwareSerial`, `SoftwareSerial`, etc.).

## Repository layout

- `/src/PMSSerial.h` / `/src/PMSSerial.cpp` - library source
- `/PMS_Serial_Test/PMS_Serial_Test.ino` - configurable example sketch (`SoftwareSerial` or `Serial1`)

## Installation

1. Download or clone this repository.
2. Place the repository folder in your Arduino libraries directory.
3. Restart Arduino IDE.
4. Include:
   ```cpp
   #include "PMSSerial.h"
   ```

## Wiring and serial settings

`PMS_Serial_Test` is compile-time configurable:

- Define `PMS_SERIAL_TEST_USE_SOFTWARE_SERIAL` to use `SoftwareSerial` on pins `13` (RX) and `12` (TX).
- Define `PMS_SERIAL_TEST_USE_SERIAL1` to use `Serial1`.

For `SoftwareSerial` mode the wiring is:

- Sensor TX -> Arduino pin `13` (SoftwareSerial RX)
- Sensor RX -> Arduino pin `12` (SoftwareSerial TX)
- Sensor baud: `9600`

On Arduino Uno R4, SoftwareSerial RX is limited to supported RX-capable pins.

If neither macro is defined, the sketch prints a setup message and does not start a sensor stream.

## API

```cpp
class PMSSerial {
public:
  struct Measurement {
    uint16_t pm1_0;
    uint16_t pm2_5;
    uint16_t pm10;
  };

  explicit PMSSerial(Stream& stream, uint8_t primaryHeader = 0x42, bool acceptAlternativeHeader = false);
  bool update();
  bool read(Measurement& outMeasurement);
  bool hasNewMeasurement() const;
  bool hasValidMeasurement() const;
  const Measurement& latestMeasurement() const;

  void setPrimaryHeader(uint8_t header);
  uint8_t primaryHeader() const;
  void setAcceptAlternativeHeader(bool enabled);
  bool acceptAlternativeHeader() const;
};
```

### Parsing behavior

- Non-blocking: call `update()` from `loop()`.
- Synchronizes on frame start bytes (`primaryHeader`, then `0x4D`).
- Reads the standard 32-byte PMS frame.
- Verifies checksum as sum of bytes `[0..29]` against bytes `[30..31]`.
- On malformed/invalid frames, parser resets and automatically resynchronizes.
- On valid frames, reads:
  - PM1.0 from bytes `[10..11]`
  - PM2.5 from bytes `[12..13]`
  - PM10 from bytes `[14..15]`

### PM2008M `0x43` note

Some PM2008M setups report `0x43` as the first header byte. To allow either `0x42` or `0x43`:

```cpp
pms.setAcceptAlternativeHeader(true);
```

This only changes first-byte synchronization. The parser still expects the same 32-byte frame layout and checksum rules.

## Usage examples

### SoftwareSerial

```cpp
#include "SoftwareSerial.h"
#include "PMSSerial.h"

SoftwareSerial pmSerial(13, 12);
PMSSerial pms(pmSerial);

void setup() {
  Serial.begin(115200);
  pmSerial.begin(9600);
}

void loop() {
  pms.update();

  PMSSerial::Measurement m;
  if (pms.read(m)) {
    Serial.print("PM2.5: ");
    Serial.println(m.pm2_5);
  }
}
```

### HardwareSerial

```cpp
#include "PMSSerial.h"

PMSSerial pms(Serial1);

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
}

void loop() {
  pms.update();
  if (pms.hasNewMeasurement()) {
    PMSSerial::Measurement m;
    if (pms.read(m)) {
      // use m.pm1_0, m.pm2_5, m.pm10
    }
  }
}
```
