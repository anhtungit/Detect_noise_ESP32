#include <web_manager.h>
#include <web_page.h>
#include <telegram_config.h>
#include <camera_manager.h>
#include <audio_recorder.h>

WebManager* WebManager::_instance = nullptr;

WebManager::WebManager()
  : _server(80), _webSocket(81), _settingsCallback(nullptr), _camManager(nullptr), _audioRecorder(nullptr) {
  _instance = this;
}

void WebManager::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (_instance) {
    _instance->handleWebSocket(num, type, payload, length);
  }
}

void WebManager::handleWebSocket(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg((char*)payload, length);

    // Đồng bộ thời gian thực từ trình duyệt vào RTC của ESP32
    int timeIdx = msg.indexOf("\"time\":");
    if (timeIdx >= 0) {
      time_t sec = (time_t)msg.substring(timeIdx + 7).toInt();
      if (sec > 1700000000) {
        struct timeval tv = { .tv_sec = sec, .tv_usec = 0 };
        settimeofday(&tv, NULL);
        Serial.printf("[WebManager] Đã đồng bộ giờ thực RTC: %lld\n", (long long)sec);
      }
    }

    // Lệnh chụp ảnh thử từ Web
    if (msg.indexOf("\"cmd\":\"capture\"") >= 0) {
      if (_camManager) {
        if (!_camManager->isInitialized()) {
          _camManager->begin();
        }
        bool ok = _camManager->capture();
        if (ok) {
          Serial.println("[WebManager] Chụp ảnh thử thành công, phát thông báo cập nhật ảnh!");
          _webSocket.broadcastTXT("{\"cam_ok\":1}");
        } else {
          char errBuf[128];
          snprintf(errBuf, sizeof(errBuf), "{\"cam_err\":1,\"code\":\"0x%x\"}", (unsigned int)_camManager->getInitError());
          _webSocket.broadcastTXT(errBuf);
          Serial.printf("[WebManager] Chụp ảnh thử thất bại, mã lỗi: 0x%x\n", (unsigned int)_camManager->getInitError());
        }
      }
    }

    // Parse "width" value
    int wIdx = msg.indexOf("\"width\":");
    int tIdx = msg.indexOf("\"thresh\":");
    if (wIdx >= 0 && tIdx >= 0) {
      // Tìm giá trị số ngay sau "width": và "thresh":
      float newW = 0.0f, newT = 0.0f;
      int wValStart = wIdx + 8;
      int tValStart = tIdx + 9;

      // Bỏ qua khoảng trắng
      while (wValStart < (int)msg.length() && msg[wValStart] == ' ') wValStart++;
      while (tValStart < (int)msg.length() && msg[tValStart] == ' ') tValStart++;

      newW = msg.substring(wValStart).toFloat();
      newT = msg.substring(tValStart).toFloat();

      if (newW > 0.0f && newT > 0.0f && _settingsCallback) {
        _settingsCallback(newW, newT);
      }
    }
  }
}

void WebManager::handleRoot() {
  _server.send(200, "text/html; charset=utf-8", INDEX_HTML);
}

void WebManager::handlePhoto() {
  if (_camManager && _camManager->hasPhoto()) {
    if (_camManager->lockPhoto(100)) {
      _server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
      _server.sendHeader("Pragma", "no-cache");
      _server.sendHeader("Expires", "-1");
      if (_server.hasArg("dl")) {
        _server.sendHeader("Content-Disposition", "attachment; filename=\"evidence.jpg\"");
      }
      _server.send_P(200, "image/jpeg", (const char*)_camManager->getLatestPhotoData(), _camManager->getLatestPhotoSize());
      _camManager->unlockPhoto();
      return;
    }
  }
  _server.send(404, "text/plain", "Chua co anh");
}

void WebManager::handleAudio() {
  if (_audioRecorder && _audioRecorder->getWavData()) {
    if (_audioRecorder->lockWav(100)) {
      _server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
      _server.sendHeader("Pragma", "no-cache");
      _server.sendHeader("Expires", "-1");
      _server.sendHeader("Content-Disposition", "inline; filename=\"evidence_3s.wav\"");
      _server.send_P(200, "audio/wav", (const char*)_audioRecorder->getWavData(), _audioRecorder->getWavSize());
      _audioRecorder->unlockWav();
      return;
    }
  }
  _server.send(404, "text/plain", "Chua co file ghi am");
}

void WebManager::handleTelegramConfig() {
  char jsonBuf[384];
  snprintf(jsonBuf, sizeof(jsonBuf),
    "{\"bot_token\":\"%s\",\"chat_id\":\"%s\",\"auto_send\":%s}",
    TELEGRAM_BOT_TOKEN,
    TELEGRAM_CHAT_ID,
    TELEGRAM_AUTO_SEND ? "true" : "false");
  _server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  _server.send(200, "application/json; charset=utf-8", jsonBuf);
}

void WebManager::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);

  Serial.println("\n-------------------------------------------------------");
  Serial.printf("[WebManager] Dang ket noi Wi-Fi: %s ...\n", WIFI_STA_SSID);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 25) {
    delay(400);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WebManager] Da ket noi Wi-Fi thanh cong!");
    Serial.print("[WebManager] Dia chi IP STA:   http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WebManager] Khong the ket noi Wi-Fi! Bat che do AP du phong...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID_NAME, AP_PASSWORD);
    delay(100);
    Serial.print("[WebManager] Dia chi Web AP:   http://");
    Serial.println(WiFi.softAPIP());
  }

  if (MDNS.begin(MDNS_HOST_NAME)) {
    Serial.printf("[WebManager] Ten mDNS:         http://%s.local\n", MDNS_HOST_NAME);
  }
  Serial.println("-------------------------------------------------------");

  _server.on("/", [this]() { this->handleRoot(); });
  _server.on("/photo.jpg", [this]() { this->handlePhoto(); });
  _server.on("/audio.wav", [this]() { this->handleAudio(); });
  _server.on("/telegram_config.json", [this]() { this->handleTelegramConfig(); });
  _server.begin();

  _webSocket.begin();
  _webSocket.onEvent(onWebSocketEvent);
  Serial.println("[WebManager] Web Server & WebSocket đã sẵn sàng!");
}

void WebManager::handle() {
  _server.handleClient();
  _webSocket.loop();
}

uint8_t WebManager::connectedClients() const {
  return const_cast<WebSocketsServer&>(_webSocket).connectedClients();
}

void WebManager::broadcastEvent(const NoiseEvent& ev) {
  if (connectedClients() == 0) return;

  char jsonBuf[384];
  snprintf(jsonBuf, sizeof(jsonBuf),
    "{\"event\":{\"db\":%.1f,\"m1\":%.1f,\"m2\":%.1f,\"tdoa\":%.1f,\"pos\":%.1f,\"pct\":%.1f,\"lane\":%u,\"photo\":1}}",
    ev.maxDb, ev.db1, ev.db2, ev.timeDiffUs, ev.positionCm, ev.positionPercent, (unsigned int)ev.lane);
  _webSocket.broadcastTXT(jsonBuf);
}

void WebManager::broadcastAudioReady() {
  if (connectedClients() == 0) return;
  _webSocket.broadcastTXT("{\"audio_ready\":1}");
}

void WebManager::broadcastLiveDb(float db1, float db2) {
  if (connectedClients() == 0) return;

  char liveBuf[128];
  snprintf(liveBuf, sizeof(liveBuf), "{\"live\":{\"m1\":%.1f,\"m2\":%.1f}}", db1, db2);
  _webSocket.broadcastTXT(liveBuf);
}

