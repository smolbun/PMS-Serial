#ifndef PMS_SERIAL_H
#define PMS_SERIAL_H

#include <Arduino.h>

class PMSSerial {
public:
  struct Measurement {
    uint16_t pm1_0;
    uint16_t pm2_5;
    uint16_t pm10;
  };

  static const uint8_t kDefaultPrimaryHeader = 0x42;
  static const uint8_t kAlternativePrimaryHeader = 0x43;
  static const uint8_t kSecondaryHeader = 0x4D;
  static const uint8_t kFrameLength = 32;

  explicit PMSSerial(Stream& stream,
                     uint8_t primaryHeader = kDefaultPrimaryHeader,
                     bool acceptAlternativeHeader = false);

  void setPrimaryHeader(uint8_t header);
  uint8_t primaryHeader() const;

  void setAcceptAlternativeHeader(bool enabled);
  bool acceptAlternativeHeader() const;

  bool update();
  bool hasNewMeasurement() const;
  bool hasValidMeasurement() const;

  bool read(Measurement& outMeasurement);
  const Measurement& latestMeasurement() const;

private:
  enum ParseState {
    WaitingForPrimaryHeader,
    WaitingForSecondaryHeader,
    ReadingFrame,
  };

  void resetParser();
  void processCompleteFrame();
  bool isAcceptedPrimaryHeader(uint8_t value) const;

  Stream& _stream;
  uint8_t _primaryHeader;
  bool _acceptAlternativeHeader;

  ParseState _state;
  uint8_t _frame[kFrameLength];
  uint8_t _frameIndex;

  Measurement _latestMeasurement;
  bool _hasValidMeasurement;
  bool _hasNewMeasurement;
};

#endif
