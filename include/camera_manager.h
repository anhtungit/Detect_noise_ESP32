#pragma once
#include <Arduino.h>
#include <esp_camera.h>
#include "config.h"

class CameraManager {
public:
  CameraManager();
  bool begin();
  bool capture();
  
  uint8_t* getLatestPhotoData();
  size_t getLatestPhotoSize();
  uint32_t getLatestPhotoTimestamp();
  bool hasPhoto();
  bool lockPhoto(uint32_t waitMs = 100);
  void unlockPhoto();
  bool isInitialized() const { return _initialized; }
  esp_err_t getInitError() const { return _lastErr; }
  uint16_t getSensorPID() const { return _sensorPID; }

private:
  bool _initialized;
  esp_err_t _lastErr;
  uint16_t _sensorPID;
  uint8_t* _latestJpegBuf;
  size_t _latestJpegSize;
  size_t _latestJpegCapacity;
  uint32_t _latestPhotoTime;
  SemaphoreHandle_t _camMutex;
};
