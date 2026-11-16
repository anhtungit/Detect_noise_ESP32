#include <Arduino.h>
#include <config.h>
#include <tdoa_processor.h>
#include <web_manager.h>
#include <camera_manager.h>
#include <audio_recorder.h>

/*
 * ====================================================================
 *   ESP32-S3: HỆ THỐNG ĐỊNH VỊ TIẾNG ỒN TRÊN ĐƯỜNG (2x INMP441)
 *   MÃ NGUỒN MODULAR - ĐÃ TÁCH THÀNH CÁC THƯ VIỆN RIÊNG BIỆT:
 *     - config.h: Cấu hình chân, Wi-Fi và các thông số
 *     - camera_manager.h/.cpp: Quản lý chụp ảnh OV3660/OV5640
 *     - audio_recorder.h/.cpp: Ghi âm bằng chứng 3s (Ring Buffer PSRAM)
 *     - web_page.h: Giao diện Web HTML5 Canvas 2D
 *     - tdoa_processor.h/.cpp: Xử lý I2S Stereo 48kHz & TDoA
 *     - web_manager.h/.cpp: Quản lý Wi-Fi, WebServer & WebSocket
 * ====================================================================
 */

TDoAProcessor tdoaProcessor;
AudioRecorder audioRecorder;

portMUX_TYPE syncMux = portMUX_INITIALIZER_UNLOCKED;
NoiseEvent syncEvent;
float syncDb1 = 0.0f;
float syncDb2 = 0.0f;

// Callback cập nhật cài đặt từ giao diện Web
void onSettingsChanged(float newRoadWidth, float newThreshold) {
  tdoaProcessor.setRoadWidth(newRoadWidth);
  tdoaProcessor.setThreshold(newThreshold);
  Serial.printf("[Main] Nhận cài đặt mới: Rộng = %.1f cm, Ngưỡng = %.1f dB\n",
    newRoadWidth, newThreshold);
}

// ===== TASK THU ÂM & TÍNH TOÁN TDoA TRÊN CORE 1 =====
void audioTask(void* param) {
  if (!tdoaProcessor.begin()) {
    while (true) delay(1000);
  }

  NoiseEvent ev;
  float db1, db2;

  while (true) {
    if (tdoaProcessor.process(ev, db1, db2)) {
      portENTER_CRITICAL(&syncMux);
      syncEvent = ev;
      syncDb1 = db1;
      syncDb2 = db2;
      portEXIT_CRITICAL(&syncMux);
    } else {
      portENTER_CRITICAL(&syncMux);
      syncDb1 = db1;
      syncDb2 = db2;
      portEXIT_CRITICAL(&syncMux);
    }
    // Đẩy mẫu âm thanh liên tục vào bộ đệm vòng ghi âm
    audioRecorder.feedSamples(tdoaProcessor.getMic1Signal(), tdoaProcessor.getMic2Signal(), AUDIO_BUFFER_SAMPLES);
    // readBytes() đã block chờ DMA đầy (~10.6ms), không cần delay thêm
  }
}

WebManager webManager;
CameraManager cameraManager;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n=======================================================");
  Serial.println("  ESP32-S3: ROAD SOUND LOCALIZATION SYSTEM (2x INMP441)");
  Serial.println("=======================================================");

  // 1. Khởi động Web Server & WebSocket trên Core 0
  webManager.setSettingsCallback(onSettingsChanged);
  webManager.setAudioRecorder(&audioRecorder);
  webManager.setCameraManager(&cameraManager);
  webManager.begin();

  // 2. Khởi tạo Camera OV3660/OV5640
  cameraManager.begin();

  // 3. Khởi tạo Bộ đệm ghi âm 3 giây trong PSRAM
  audioRecorder.begin();

  // 4. Khởi chạy Audio TDoA Task trên Core 1
  xTaskCreatePinnedToCore(
    audioTask,
    "TDoA_AudioTask",
    8192,
    NULL,
    1,
    NULL,
    1 // Ghim Core 1
  );

  Serial.println("=======================================================\n");
}

unsigned long lastLiveSend = 0;

void loop() {
  // Web Server phản hồi trên Core 0
  webManager.handle();

  unsigned long now = millis();

  // 1. Bắn gói tin sự kiện tức thời khi có tiếng ồn vượt ngưỡng
  bool hasEvent = false;
  NoiseEvent evCopy;
  float liveM1, liveM2;

  portENTER_CRITICAL(&syncMux);
  if (syncEvent.triggered) {
    evCopy = syncEvent;
    syncEvent.triggered = false;
    hasEvent = true;
  }
  liveM1 = syncDb1;
  liveM2 = syncDb2;
  portEXIT_CRITICAL(&syncMux);

  if (hasEvent) {
    // 1. Chụp ảnh bằng camera ngay khi phát hiện tiếng ồn vượt ngưỡng
    cameraManager.capture();
    // 2. Kích hoạt thu 1.5s tiếp theo (để tạo đoạn 3s quanh thời điểm vi phạm)
    audioRecorder.trigger();
    webManager.broadcastEvent(evCopy);
  }

  // Khi bộ ghi âm hoàn tất đóng gói file WAV 3 giây -> báo cho Web client
  if (audioRecorder.isReady()) {
    audioRecorder.clearReady();
    webManager.broadcastAudioReady();
  }

  // 2. Gửi định kỳ mức dB trực tiếp (10 lần/giây)
  if (now - lastLiveSend >= 100) {
    lastLiveSend = now;
    webManager.broadcastLiveDb(liveM1, liveM2);
  }
}
