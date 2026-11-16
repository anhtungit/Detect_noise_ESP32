#include "audio_recorder.h"

AudioRecorder::AudioRecorder()
  : _initialized(false),
    _ringBuffer(nullptr),
    _ringHead(0),
    _isCapturing(false),
    _postRemaining(0),
    _ready(false),
    _wavBuffer(nullptr),
    _mux(portMUX_INITIALIZER_UNLOCKED),
    _wavMutex(nullptr) {
  _wavMutex = xSemaphoreCreateMutex();
}

bool AudioRecorder::begin() {
  size_t ringBytes = REC_TOTAL_SAMPLES * sizeof(int16_t);
  size_t wavBytes  = WAV_TOTAL_SIZE;

  if (psramFound()) {
    _ringBuffer = (int16_t*)ps_malloc(ringBytes);
    _wavBuffer  = (uint8_t*)ps_malloc(wavBytes);
  } else {
    _ringBuffer = (int16_t*)malloc(ringBytes);
    _wavBuffer  = (uint8_t*)malloc(wavBytes);
  }

  if (!_ringBuffer || !_wavBuffer) {
    Serial.println("[AudioRecorder] LỖI: Không thể cấp phát bộ đệm ghi âm trong PSRAM!");
    _initialized = false;
    return false;
  }

  memset(_ringBuffer, 0, ringBytes);
  memset(_wavBuffer, 0, wavBytes);

  // Ghi sẵn WAV header 44 byte
  writeWavHeader(_wavBuffer, REC_TOTAL_SAMPLES * sizeof(int16_t), REC_SAMPLE_RATE, 1, 16);

  _initialized = true;
  Serial.printf("[AudioRecorder] Đã cấp phát bộ đệm 3s trong PSRAM (%lu KB) thành công!\n",
    (unsigned long)((ringBytes + wavBytes) / 1024));
  return true;
}

void AudioRecorder::writeWavHeader(uint8_t* buf, uint32_t pcmDataBytes, uint32_t sampleRate, uint16_t channels, uint16_t bitsPerSample) {
  uint32_t totalFileSize = pcmDataBytes + 36;
  uint32_t byteRate = sampleRate * channels * (bitsPerSample / 8);
  uint16_t blockAlign = channels * (bitsPerSample / 8);

  // RIFF
  buf[0] = 'R'; buf[1] = 'I'; buf[2] = 'F'; buf[3] = 'F';
  memcpy(&buf[4], &totalFileSize, 4);
  buf[8] = 'W'; buf[9] = 'A'; buf[10] = 'V'; buf[11] = 'E';

  // fmt
  buf[12] = 'f'; buf[13] = 'm'; buf[14] = 't'; buf[15] = ' ';
  uint32_t sub1 = 16;
  memcpy(&buf[16], &sub1, 4);
  uint16_t format = 1; // PCM
  memcpy(&buf[20], &format, 2);
  memcpy(&buf[22], &channels, 2);
  memcpy(&buf[24], &sampleRate, 4);
  memcpy(&buf[28], &byteRate, 4);
  memcpy(&buf[32], &blockAlign, 2);
  memcpy(&buf[34], &bitsPerSample, 2);

  // data
  buf[36] = 'd'; buf[37] = 'a'; buf[38] = 't'; buf[39] = 'a';
  memcpy(&buf[40], &pcmDataBytes, 4);
}

void AudioRecorder::feedSamples(const float* signal1, const float* signal2, int count) {
  if (!_initialized) return;

  bool shouldExport = false;
  int snapshotHead = 0;

  portENTER_CRITICAL(&_mux);
  for (int i = 0; i < count; i++) {
    // Trộn 2 micro và chuyển đổi 24-bit sang 16-bit PCM
    float mix = (signal1[i] + signal2[i]) * 0.5f;
    int32_t s16 = (int32_t)(mix / 256.0f);
    _ringBuffer[_ringHead] = (int16_t)constrain(s16, -32768, 32767);
    _ringHead = (_ringHead + 1) % REC_TOTAL_SAMPLES;
  }

  if (_isCapturing) {
    _postRemaining -= count;
    if (_postRemaining <= 0) {
      _isCapturing = false;
      shouldExport = true;
      snapshotHead = _ringHead;
    }
  }
  portEXIT_CRITICAL(&_mux);

  if (shouldExport) {
    exportWav(snapshotHead);
    portENTER_CRITICAL(&_mux);
    _ready = true;
    portEXIT_CRITICAL(&_mux);
  }
}

void AudioRecorder::trigger() {
  if (!_initialized) return;

  portENTER_CRITICAL(&_mux);
  // Bắt đầu thu 1.5s tiếp theo (hệ thống đã có sẵn 1.5s trước đó trong ring buffer)
  _isCapturing = true;
  _postRemaining = REC_POST_SAMPLES;
  portEXIT_CRITICAL(&_mux);

  Serial.println("[AudioRecorder] Đã kích hoạt ghi âm 3s (1.5s trước + 1.5s sau tiếng ồn)...");
}

void AudioRecorder::exportWav(int snapshotHead) {
  if (lockWav(500)) {
    // Trích xuất tuần tự 144,000 mẫu bắt đầu từ điểm cũ nhất (snapshotHead)
    int16_t* pcmDest = (int16_t*)(_wavBuffer + WAV_HEADER_SIZE);
    int firstPart = REC_TOTAL_SAMPLES - snapshotHead;
    int secondPart = snapshotHead;

    memcpy(pcmDest, &_ringBuffer[snapshotHead], firstPart * sizeof(int16_t));
    if (secondPart > 0) {
      memcpy(pcmDest + firstPart, &_ringBuffer[0], secondPart * sizeof(int16_t));
    }
    unlockWav();
  }
  Serial.println("[AudioRecorder] Đã hoàn tất đóng gói file WAV 3 giây!");
}

bool AudioRecorder::isReady() {
  portENTER_CRITICAL(&_mux);
  bool r = _ready;
  portEXIT_CRITICAL(&_mux);
  return r;
}

void AudioRecorder::clearReady() {
  portENTER_CRITICAL(&_mux);
  _ready = false;
  portEXIT_CRITICAL(&_mux);
}

bool AudioRecorder::lockWav(uint32_t waitMs) {
  if (!_wavMutex) return false;
  return (xSemaphoreTake(_wavMutex, pdMS_TO_TICKS(waitMs)) == pdTRUE);
}

void AudioRecorder::unlockWav() {
  if (_wavMutex) {
    xSemaphoreGive(_wavMutex);
  }
}

uint8_t* AudioRecorder::getWavData() {
  return _wavBuffer;
}

size_t AudioRecorder::getWavSize() const {
  return WAV_TOTAL_SIZE;
}
