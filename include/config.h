#pragma once
#include <Arduino.h>

// ===== CẤU HÌNH PHẦN CỨNG I2S =====
// Dùng chung cho 2 micro INMP441 (Stereo)
#define I2S0_SCK_PIN          21    // BCLK
#define I2S0_WS_PIN           47    // LRCK / WS
#define I2S0_SD_PIN           41   // DIN / SD

// ===== CẤU HÌNH CAMERA (ESP32-S3 N16R8 Cam Board - OV3660 / OV5640) =====
#define CAM_PIN_PWDN          -1
#define CAM_PIN_RESET         -1
#define CAM_PIN_XCLK          15
#define CAM_PIN_SIOD          4    // I2C SDA
#define CAM_PIN_SIOC          5    // I2C SCL

#define CAM_PIN_D7            16   // Y9
#define CAM_PIN_D6            17   // Y8
#define CAM_PIN_D5            18   // Y7
#define CAM_PIN_D4            12   // Y6
#define CAM_PIN_D3            10   // Y5
#define CAM_PIN_D2            8    // Y4
#define CAM_PIN_D1            9    // Y3
#define CAM_PIN_D0            11   // Y2

#define CAM_PIN_VSYNC         6
#define CAM_PIN_HREF          7
#define CAM_PIN_PCLK          13

// Cấu hình kênh INMP441:
// - Mic 1 (Lề Trái): chân L/R nối GND  (Left)
// - Mic 2 (Lề Phải): chân L/R nối 3.3V (Right)

// ===== CẤU HÌNH WI-FI (STATION & DỰ PHÒNG AP) =====
#define WIFI_STA_SSID         "local_nitro5"
#define WIFI_STA_PASS         "12345678"
#define AP_SSID_NAME          "ESP32-Audio-Spectrum"
#define AP_PASSWORD           "12345678"
#define MDNS_HOST_NAME        "esp32"

// ===== CẤU HÌNH ÂM THANH & TDoA =====
#define AUDIO_SAMPLE_RATE     48000     // 48 kHz
#define AUDIO_BUFFER_SAMPLES  512       // Khối mẫu thu mỗi chu kỳ (~10.6ms)
#define SPEED_OF_SOUND_CM_S   34300.0f  // Vận tốc âm thanh: 343 m/s = 34300 cm/s

// Cấu hình mặc định (có thể chỉnh động trên Web)
#define DEFAULT_ROAD_WIDTH_CM   30.0f   // Bề rộng mô hình đường: 30 cm
#define DEFAULT_THRESHOLD_DB    75.0f   // Ngưỡng phát hiện tiếng ồn: 75 dB
#define NOISE_AVG_COUNT         30      // Số lần bắt mẫu vượt ngưỡng để lấy trung bình
#define NOISE_BURST_TIMEOUT_MS  1000    // Quá thời gian này không có mẫu mới -> reset bộ đếm

// Phân tách 3 làn đường (tính theo % từ Mic 1 sang Mic 2, khớp 2 vạch băng keo)
// Làn 1 (Trái):  0%    -> 38.5%
// Làn 2 (Giữa):  38.5% -> 74.0%
// Làn 3 (Phải):  74.0% -> 100%
#define LANE_1_MAX_PCT          38.5f
#define LANE_2_MAX_PCT          74.0f

enum RoadLane : uint8_t {
  LANE_UNKNOWN = 0,
  LANE_LEFT    = 1,   // Làn 1: Trái
  LANE_CENTER  = 2,   // Làn 2: Giữa
  LANE_RIGHT   = 3    // Làn 3: Phải
};

// Cấu trúc dữ liệu sự kiện tiếng ồn vượt ngưỡng
struct NoiseEvent {
  bool triggered;
  float maxDb;
  float db1;
  float db2;
  float timeDiffUs;      // Độ lệch thời gian (micro-giây)
  float positionCm;      // Vị trí tiếng ồn tính từ Mic 1 (cm)
  float positionPercent; // Vị trí % (0% = Mic 1, 100% = Mic 2)
  uint8_t lane;          // Phân loại làn (1: Trái, 2: Giữa, 3: Phải)
  uint32_t timestamp;
};

