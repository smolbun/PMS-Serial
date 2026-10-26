#include "PMSSerial.h"

PMSSerial::PMSSerial(Stream& stream,
                     uint8_t primaryHeader,
                     bool acceptAlternativeHeader)
    : _stream(stream),
      _primaryHeader(primaryHeader),
      _acceptAlternativeHeader(acceptAlternativeHeader),
      _state(WaitingForPrimaryHeader),
      _frameIndex(0),
      _latestMeasurement{0, 0, 0},
      _hasValidMeasurement(false),
      _hasNewMeasurement(false) {}

void PMSSerial::setPrimaryHeader(uint8_t header) {
  _primaryHeader = header;
  resetParser();
}

uint8_t PMSSerial::primaryHeader() const {
  return _primaryHeader;
}

void PMSSerial::setAcceptAlternativeHeader(bool enabled) {
  _acceptAlternativeHeader = enabled;
}

bool PMSSerial::acceptAlternativeHeader() const {
  return _acceptAlternativeHeader;
}

bool PMSSerial::update() {
  bool gotNewMeasurement = false;

  while (_stream.available() > 0) {
    const int value = _stream.read();
    if (value < 0) {
      continue;
    }

    const uint8_t incomingByte = static_cast<uint8_t>(value);

    if (_state == WaitingForPrimaryHeader) {
      if (isAcceptedPrimaryHeader(incomingByte)) {
        _frame[0] = incomingByte;
        _frameIndex = 1;
        _state = WaitingForSecondaryHeader;
      }
      continue;
    }

    if (_state == WaitingForSecondaryHeader) {
      if (incomingByte == kSecondaryHeader) {
        _frame[1] = incomingByte;
        _frameIndex = 2;
        _state = ReadingFrame;
      } else if (isAcceptedPrimaryHeader(incomingByte)) {
        _frame[0] = incomingByte;
        _frameIndex = 1;
      } else {
        resetParser();
      }
      continue;
    }

    _frame[_frameIndex++] = incomingByte;
    if (_frameIndex == kFrameLength) {
      processCompleteFrame();
      gotNewMeasurement = _hasNewMeasurement;
      resetParser();
    }
  }

  return gotNewMeasurement;
}

bool PMSSerial::hasNewMeasurement() const {
  return _hasNewMeasurement;
}

bool PMSSerial::hasValidMeasurement() const {
  return _hasValidMeasurement;
}

bool PMSSerial::read(Measurement& outMeasurement) {
  if (!_hasNewMeasurement) {
    return false;
  }

  outMeasurement = _latestMeasurement;
  _hasNewMeasurement = false;
  return true;
}

const PMSSerial::Measurement& PMSSerial::latestMeasurement() const {
  return _latestMeasurement;
}

void PMSSerial::resetParser() {
  _state = WaitingForPrimaryHeader;
  _frameIndex = 0;
}

bool PMSSerial::isAcceptedPrimaryHeader(uint8_t value) const {
  return value == _primaryHeader || (_acceptAlternativeHeader && value == kAlternativePrimaryHeader);
}

void PMSSerial::processCompleteFrame() {
  uint16_t sum = 0;
  for (uint8_t i = 0; i < 30; ++i) {
    sum += _frame[i];
  }

  const uint16_t checksum = (static_cast<uint16_t>(_frame[30]) << 8) | _frame[31];
  if (sum != checksum) {
    return;
  }

  _latestMeasurement.pm1_0 = (static_cast<uint16_t>(_frame[10]) << 8) | _frame[11];
  _latestMeasurement.pm2_5 = (static_cast<uint16_t>(_frame[12]) << 8) | _frame[13];
  _latestMeasurement.pm10 = (static_cast<uint16_t>(_frame[14]) << 8) | _frame[15];
  _hasValidMeasurement = true;
  _hasNewMeasurement = true;
}
