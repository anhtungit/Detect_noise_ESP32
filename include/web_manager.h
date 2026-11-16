#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include "config.h"

class CameraManager;
class AudioRecorder;

typedef void (*SettingsCallback)(float newRoadWidth, float newThreshold);

class WebManager {
public:
  WebManager();
  void begin();
  void handle();

  void setSettingsCallback(SettingsCallback cb) { _settingsCallback = cb; }
  void setCameraManager(CameraManager* cam) { _camManager = cam; }
  void setAudioRecorder(AudioRecorder* rec) { _audioRecorder = rec; }

  void broadcastEvent(const NoiseEvent& ev);
  void broadcastLiveDb(float db1, float db2);
  void broadcastAudioReady();
  uint8_t connectedClients() const;

private:
  WebServer _server;
  WebSocketsServer _webSocket;
  SettingsCallback _settingsCallback;
  CameraManager* _camManager;
  AudioRecorder* _audioRecorder;

  static WebManager* _instance;
  static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
  void handleWebSocket(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
  void handleRoot();
  void handlePhoto();
  void handleAudio();
  void handleTelegramConfig();
};

