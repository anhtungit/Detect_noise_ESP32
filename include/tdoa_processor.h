#pragma once
#include <Arduino.h>
#include <ESP_I2S.h>
#include "config.h"

class TDoAProcessor {
public:
  TDoAProcessor();
  bool begin();
  
  // Xử lý một chu kỳ âm thanh và phát hiện tiếng ồn
  bool process(NoiseEvent& outEvent, float& outLiveDb1, float& outLiveDb2);

  void setRoadWidth(float widthCm) { _roadWidthCm = widthCm; }
  void setThreshold(float threshDb) { _thresholdDb = threshDb; }

  float getRoadWidth() const { return _roadWidthCm; }
  float getThreshold() const { return _thresholdDb; }

  const float* getMic1Signal() const { return _mic1Signal; }
  const float* getMic2Signal() const { return _mic2Signal; }

private:
  I2SClass _i2s;
  bool _ready;
  float _roadWidthCm;
  float _thresholdDb;

  // Bộ tích lũy để lấy trung bình
  int _accumCount;
  float _accumMaxDb;
  float _accumDb1;
  float _accumDb2;
  float _accumTimeDiffUs;
  float _accumPosCm;
  uint32_t _lastTriggerTime;

  int32_t _rawBuffer[AUDIO_BUFFER_SAMPLES * 2];
  float _mic1Signal[AUDIO_BUFFER_SAMPLES];
  float _mic2Signal[AUDIO_BUFFER_SAMPLES];

  // Thuật toán Tương Quan Chéo (Cross-Correlation)
  int calculateBestLag(int maxDelaySamples, float& outMaxCorrelation);
};

