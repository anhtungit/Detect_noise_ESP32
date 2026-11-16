#include "camera_manager.h"

CameraManager::CameraManager()
  : _initialized(false),
    _lastErr(ESP_OK),
    _sensorPID(0),
    _latestJpegBuf(nullptr),
    _latestJpegSize(0),
    _latestJpegCapacity(0),
    _latestPhotoTime(0),
    _camMutex(nullptr) {
  _camMutex = xSemaphoreCreateMutex();
}

bool CameraManager::begin() {
  camera_config_t config;
  memset(&config, 0, sizeof(camera_config_t));

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = CAM_PIN_D0;
  config.pin_d1       = CAM_PIN_D1;
  config.pin_d2       = CAM_PIN_D2;
  config.pin_d3       = CAM_PIN_D3;
  config.pin_d4       = CAM_PIN_D4;
  config.pin_d5       = CAM_PIN_D5;
  config.pin_d6       = CAM_PIN_D6;
  config.pin_d7       = CAM_PIN_D7;
  config.pin_xclk     = CAM_PIN_XCLK;
  config.pin_pclk     = CAM_PIN_PCLK;
  config.pin_vsync    = CAM_PIN_VSYNC;
  config.pin_href     = CAM_PIN_HREF;
  config.pin_sccb_sda = CAM_PIN_SIOD;
  config.pin_sccb_scl = CAM_PIN_SIOC;
  config.pin_pwdn     = CAM_PIN_PWDN;
  config.pin_reset    = CAM_PIN_RESET;
  config.sccb_i2c_port = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Dùng PSRAM để lưu frame buffer độ phân giải VGA 640x480
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 14;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[Camera] Thử 20MHz thất bại (0x%x). Thử lại ở xung 10MHz...\n", err);
    esp_camera_deinit();
    delay(100);
    config.xclk_freq_hz = 10000000;
    err = esp_camera_init(&config);
  }

  if (err != ESP_OK) {
    Serial.printf("[Camera] LỖI KHỞI TẠO CAMERA: 0x%x\n", err);
    _lastErr = err;
    _initialized = false;
    return false;
  }

  sensor_t * s = esp_camera_sensor_get();
  if (s != nullptr) {
    _sensorPID = s->id.PID;
    Serial.printf("[Camera] Đã nhận diện cảm biến ảnh PID: 0x%02X\n", _sensorPID);
    // Tự động tối ưu độ sáng & tương phản cho cảm biến OV3660 / OV5640 / OV2640
    s->set_brightness(s, 1);
    s->set_contrast(s, 1);
  }

  // Chụp thử 1 frame khởi động (warm-up)
  delay(100);
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb) {
    esp_camera_fb_return(fb);
    Serial.println("[Camera] Warm-up frame thành công! Camera đã sẵn sàng.");
  }

  // Cấp phát trước bộ đệm JPEG trong PSRAM để tránh alloc khi capture
  if (_latestJpegBuf == nullptr) {
    size_t initialCap = 128 * 1024; // 128 KB
    _latestJpegBuf = psramFound() ? (uint8_t*)ps_malloc(initialCap) : (uint8_t*)malloc(initialCap);
    if (_latestJpegBuf) {
      _latestJpegCapacity = initialCap;
    }
  }

  _lastErr = ESP_OK;
  _initialized = true;
  return true;
}

bool CameraManager::lockPhoto(uint32_t waitMs) {
  if (!_camMutex) return false;
  return (xSemaphoreTake(_camMutex, pdMS_TO_TICKS(waitMs)) == pdTRUE);
}

void CameraManager::unlockPhoto() {
  if (_camMutex) {
    xSemaphoreGive(_camMutex);
  }
}

bool CameraManager::capture() {
  if (!_initialized) return false;

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[Camera] Lỗi: Không thể chụp frame ảnh!");
    return false;
  }

  if (lockPhoto(200)) {
    if (fb->len > _latestJpegCapacity) {
      if (_latestJpegBuf != nullptr) {
        free(_latestJpegBuf);
        _latestJpegBuf = nullptr;
      }
      size_t newCap = fb->len + 8192;
      _latestJpegBuf = psramFound() ? (uint8_t*)ps_malloc(newCap) : (uint8_t*)malloc(newCap);
      _latestJpegCapacity = (_latestJpegBuf != nullptr) ? newCap : 0;
    }

    if (_latestJpegBuf != nullptr) {
      memcpy(_latestJpegBuf, fb->buf, fb->len);
      _latestJpegSize = fb->len;
      _latestPhotoTime = millis();
    }
    unlockPhoto();
  }

  esp_camera_fb_return(fb);
  Serial.printf("[Camera] Đã chụp ảnh thành công! Dung lượng: %u bytes\n", _latestJpegSize);
  return true;
}

uint8_t* CameraManager::getLatestPhotoData() {
  return _latestJpegBuf;
}

size_t CameraManager::getLatestPhotoSize() {
  return _latestJpegSize;
}

uint32_t CameraManager::getLatestPhotoTimestamp() {
  return _latestPhotoTime;
}

bool CameraManager::hasPhoto() {
  return (_latestJpegSize > 0 && _latestJpegBuf != nullptr);
}
