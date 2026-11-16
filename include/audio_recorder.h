#pragma once
#include <Arduino.h>
#include "config.h"

#define REC_SAMPLE_RATE         48000
#define REC_DURATION_SEC        3
#define REC_TOTAL_SAMPLES       (REC_SAMPLE_RATE * REC_DURATION_SEC) // 144,000 samples (~3 giây)
#define REC_POST_SAMPLES        (REC_SAMPLE_RATE * 3 / 2)             // 72,000 samples (1.5 giây sau trigger)
#define WAV_HEADER_SIZE         44
#define WAV_TOTAL_SIZE          (WAV_HEADER_SIZE + REC_TOTAL_SAMPLES * sizeof(int16_t)) // 288,044 bytes

class AudioRecorder {
public:
  AudioRecorder();
  bool begin();
  
  // Nạp các mẫu âm thanh (gọi từ audioTask Core 1)
  void feedSamples(const float* signal1, const float* signal2, int count);

  // Kích hoạt ghi âm khi có tiếng ồn vượt ngưỡng
  void trigger();

  // Kiểm tra xem đã hoàn tất đóng gói 3s chưa
  bool isReady();
  void clearReady();

  uint8_t* getWavData();
  size_t getWavSize() const;
  bool lockWav(uint32_t waitMs = 100);
  void unlockWav();

private:
  bool _initialized;
  int16_t* _ringBuffer;
  int _ringHead;

  bool _isCapturing;
  int _postRemaining;
  volatile bool _ready;

  uint8_t* _wavBuffer;
  portMUX_TYPE _mux;
  SemaphoreHandle_t _wavMutex;

  void writeWavHeader(uint8_t* buf, uint32_t pcmDataBytes, uint32_t sampleRate, uint16_t channels, uint16_t bitsPerSample);
  void exportWav(int snapshotHead);
};
