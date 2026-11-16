#include <tdoa_processor.h>

TDoAProcessor::TDoAProcessor()
  : _i2s(I2S_NUM_0), _ready(false),
    _roadWidthCm(DEFAULT_ROAD_WIDTH_CM),
    _thresholdDb(DEFAULT_THRESHOLD_DB),
    _accumCount(0),
    _accumMaxDb(0.0f),
    _accumDb1(0.0f),
    _accumDb2(0.0f),
    _accumTimeDiffUs(0.0f),
    _accumPosCm(0.0f),
    _lastTriggerTime(0) {}

bool TDoAProcessor::begin() {
  _i2s.setPins(I2S0_SCK_PIN, I2S0_WS_PIN, -1, I2S0_SD_PIN);
  if (_i2s.begin(I2S_MODE_STD, AUDIO_SAMPLE_RATE, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)) {
    _ready = true;
    Serial.println("[TDoAProcessor] Khởi tạo I2S Stereo 48kHz thành công!");
    return true;
  }
  Serial.println("[TDoAProcessor] LỖI: Không thể khởi tạo I2S Stereo!");
  return false;
}

int TDoAProcessor::calculateBestLag(int maxDelaySamples, float& outMaxCorrelation) {
  float maxCorr = -1e20f;
  int bestLag = 0;

  for (int lag = -maxDelaySamples; lag <= maxDelaySamples; lag++) {
    float corr = 0.0f;
    float sumSq1 = 0.0f;
    float sumSq2 = 0.0f;
    int startIdx = (lag < 0) ? -lag : 0;
    int endIdx = (lag > 0) ? (AUDIO_BUFFER_SAMPLES - lag) : AUDIO_BUFFER_SAMPLES;

    for (int i = startIdx; i < endIdx; i++) {
      float v1 = _mic1Signal[i];
      float v2 = _mic2Signal[i + lag];
      corr += v1 * v2;
      sumSq1 += v1 * v1;
      sumSq2 += v2 * v2;
    }

    float norm = sqrt(sumSq1 * sumSq2);
    if (norm > 1.0f) {
      corr /= norm;
    }

    if (corr > maxCorr) {
      maxCorr = corr;
      bestLag = lag;
    }
  }

  outMaxCorrelation = maxCorr;
  return bestLag;
}

bool TDoAProcessor::process(NoiseEvent& outEvent, float& outLiveDb1, float& outLiveDb2) {
  if (!_ready) return false;

  size_t bytesRead = _i2s.readBytes((char*)_rawBuffer, sizeof(_rawBuffer));
  int samplesRead = bytesRead / (sizeof(int32_t) * 2);

  if (samplesRead != AUDIO_BUFFER_SAMPLES) return false;

  // 1. Tách tín hiệu Stereo & Khử DC Offset
  int64_t sum1 = 0, sum2 = 0;
  for (int i = 0; i < AUDIO_BUFFER_SAMPLES; i++) {
    sum1 += (_rawBuffer[i * 2] >> 8);
    sum2 += (_rawBuffer[i * 2 + 1] >> 8);
  }
  int32_t mean1 = sum1 / AUDIO_BUFFER_SAMPLES;
  int32_t mean2 = sum2 / AUDIO_BUFFER_SAMPLES;

  int64_t sumSq1 = 0, sumSq2 = 0;
  for (int i = 0; i < AUDIO_BUFFER_SAMPLES; i++) {
    float v1 = (float)((_rawBuffer[i * 2] >> 8) - mean1);
    float v2 = (float)((_rawBuffer[i * 2 + 1] >> 8) - mean2);
    _mic1Signal[i] = v1;
    _mic2Signal[i] = v2;
    sumSq1 += (int64_t)(v1 * v1);
    sumSq2 += (int64_t)(v2 * v2);
  }

  // 2. Tính mức dB của 2 mic (chuẩn dBSPL theo datasheet INMP441: -26 dBFS tại 94 dBSPL => 0 dBFS = 120 dBSPL)
  float rms1 = sqrt((float)sumSq1 / AUDIO_BUFFER_SAMPLES);
  float rms2 = sqrt((float)sumSq2 / AUDIO_BUFFER_SAMPLES);
  float dbFS1 = 20.0f * log10(max(rms1, 1.0f) / 8388608.0f);
  float dbFS2 = 20.0f * log10(max(rms2, 1.0f) / 8388608.0f);
  float db1 = constrain(dbFS1 + 120.0f, 0.0f, 130.0f);
  float db2 = constrain(dbFS2 + 120.0f, 0.0f, 130.0f);
  float maxDb = max(db1, db2);

  outLiveDb1 = db1;
  outLiveDb2 = db2;

  // 3. Kiểm tra ngưỡng phát hiện tiếng ồn
  if (maxDb >= _thresholdDb) {
    uint32_t now = millis();
    // Quá thời gian chờ giữa 2 lần bắt -> reset bộ đếm tích lũy
    if (_accumCount > 0 && (now - _lastTriggerTime > NOISE_BURST_TIMEOUT_MS)) {
      _accumCount = 0;
      _accumMaxDb = 0.0f;
      _accumDb1 = 0.0f;
      _accumDb2 = 0.0f;
      _accumTimeDiffUs = 0.0f;
      _accumPosCm = 0.0f;
    }
    _lastTriggerTime = now;

    int maxDelaySamples = (int)ceil((_roadWidthCm / SPEED_OF_SOUND_CM_S) * AUDIO_SAMPLE_RATE) + 4;
    if (maxDelaySamples > 60) maxDelaySamples = 60;

    float maxCorr = 0.0f;
    int bestLag = calculateBestLag(maxDelaySamples, maxCorr);

    float timeDiffSec = (float)bestLag / (float)AUDIO_SAMPLE_RATE;
    float timeDiffUs = timeDiffSec * 1000000.0f;

    // bestLag > 0 => Mic1 nhận trước => nguồn gần Mic1 => posCm nhỏ
    // Công thức: d1 = (W - v*Δt) / 2
    float posCm = (_roadWidthCm - (SPEED_OF_SOUND_CM_S * timeDiffSec)) / 2.0f;

    // Tinh chỉnh nhẹ theo chênh lệch biên độ (ILD)
    float dbDiff = db1 - db2;
    float ampWeight = constrain(dbDiff * 0.4f, -_roadWidthCm * 0.25f, _roadWidthCm * 0.25f);
    posCm -= ampWeight;

    posCm = constrain(posCm, 0.0f, _roadWidthCm);

    // Tích lũy giá trị
    _accumMaxDb += maxDb;
    _accumDb1 += db1;
    _accumDb2 += db2;
    _accumTimeDiffUs += timeDiffUs;
    _accumPosCm += posCm;
    _accumCount++;

    // Bắt đủ NOISE_AVG_COUNT lần (30 lần) mới tính trung bình và kích hoạt sự kiện
    if (_accumCount >= NOISE_AVG_COUNT) {
      float avgMaxDb = _accumMaxDb / (float)NOISE_AVG_COUNT;
      float avgDb1 = _accumDb1 / (float)NOISE_AVG_COUNT;
      float avgDb2 = _accumDb2 / (float)NOISE_AVG_COUNT;
      float avgTimeDiffUs = _accumTimeDiffUs / (float)NOISE_AVG_COUNT;
      float avgPosCm = _accumPosCm / (float)NOISE_AVG_COUNT;
      float avgPosPercent = (avgPosCm / _roadWidthCm) * 100.0f;

      uint8_t lane = LANE_CENTER;
      if (avgPosPercent < LANE_1_MAX_PCT) {
        lane = LANE_LEFT;
      } else if (avgPosPercent > LANE_2_MAX_PCT) {
        lane = LANE_RIGHT;
      }

      outEvent.triggered = true;
      outEvent.maxDb = avgMaxDb;
      outEvent.db1 = avgDb1;
      outEvent.db2 = avgDb2;
      outEvent.timeDiffUs = avgTimeDiffUs;
      outEvent.positionCm = avgPosCm;
      outEvent.positionPercent = avgPosPercent;
      outEvent.lane = lane;
      outEvent.timestamp = now;

      const char* laneNames[] = {"KHÔNG RÕ", "LÀN 1 (TRÁI)", "LÀN 2 (GIỮA)", "LÀN 3 (PHẢI)"};
      Serial.printf("[TDoA] (TB %d lần) Tiếng ồn: %.1f dB | Δt: %.1f us | Vị trí: %.1f cm / %.1f cm (%.1f%%) => %s\n",
        NOISE_AVG_COUNT, avgMaxDb, avgTimeDiffUs, avgPosCm, _roadWidthCm, avgPosPercent, laneNames[lane]);

      // Reset cho lần bắt tiếp theo
      _accumCount = 0;
      _accumMaxDb = 0.0f;
      _accumDb1 = 0.0f;
      _accumDb2 = 0.0f;
      _accumTimeDiffUs = 0.0f;
      _accumPosCm = 0.0f;

      return true;
    }
  }

  return false;
}

