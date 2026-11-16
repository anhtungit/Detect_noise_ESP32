#pragma once
#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Định Vị Tiếng Ồn Trên Làn Đường - 2x INMP441</title>
  <style>
    :root {
      --bg: #080d17;
      --card: #10192b;
      --border: #1e293b;
      --accent: #00f0ff;
      --danger: #ff0055;
      --warn: #ffb703;
      --green: #00ff88;
      --text: #f1f5f9;
      --dim: #94a3b8;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      background: var(--bg);
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", sans-serif;
      padding: 14px;
      display: flex;
      flex-direction: column;
      align-items: center;
      min-height: 100vh;
    }
    .container {
      width: 100%;
      max-width: 960px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }

    /* Header & Tabs */
    .header {
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 12px 16px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
      gap: 10px;
    }
    .title h1 {
      font-size: 1.1rem;
      font-weight: 700;
      letter-spacing: 0.3px;
    }
    .title h1 span { color: var(--accent); }
    .header-right {
      display: flex;
      align-items: center;
      gap: 10px;
      flex-wrap: wrap;
    }
    .badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      font-size: 0.78rem;
      padding: 5px 10px;
      border-radius: 999px;
      background: rgba(0, 240, 255, 0.08);
      color: var(--accent);
      border: 1px solid rgba(0, 240, 255, 0.25);
    }
    .dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background: var(--green);
      box-shadow: 0 0 6px var(--green);
    }
    .dot.offline {
      background: var(--danger);
      box-shadow: 0 0 6px var(--danger);
    }

    .tabs-bar {
      display: flex;
      gap: 8px;
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 10px;
      padding: 6px;
    }
    .tab-btn {
      flex: 1;
      background: transparent;
      color: var(--dim);
      border: 1px solid transparent;
      border-radius: 8px;
      padding: 8px 14px;
      font-size: 0.85rem;
      font-weight: 600;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      transition: background 0.15s, color 0.15s, border-color 0.15s;
    }
    .tab-btn:hover {
      color: var(--text);
      background: rgba(255, 255, 255, 0.03);
    }
    .tab-btn.active {
      background: rgba(0, 240, 255, 0.12);
      color: var(--accent);
      border-color: rgba(0, 240, 255, 0.35);
    }
    .tab-count {
      background: rgba(255, 0, 85, 0.18);
      color: var(--danger);
      border: 1px solid rgba(255, 0, 85, 0.35);
      border-radius: 999px;
      padding: 1px 7px;
      font-size: 0.72rem;
      font-family: monospace;
    }
    .tab-pane {
      display: none;
      flex-direction: column;
      gap: 12px;
    }
    .tab-pane.active {
      display: flex;
    }

    /* Alert Banner */
    .alert-banner {
      display: none;
      background: rgba(255, 0, 85, 0.12);
      border: 1px solid var(--danger);
      border-radius: 10px;
      padding: 10px 14px;
      color: #ff4d88;
      font-size: 0.86rem;
      font-weight: 600;
      align-items: center;
      justify-content: space-between;
      gap: 8px;
    }

    /* Card & Sections */
    .card {
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 14px 16px;
      display: flex;
      flex-direction: column;
      gap: 10px;
    }
    .section-head {
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
      gap: 8px;
    }
    .card-title {
      font-size: 0.76rem;
      color: var(--dim);
      text-transform: uppercase;
      letter-spacing: 0.5px;
      font-weight: 700;
    }

    /* Road Simulation */
    .road-canvas-wrap {
      width: 100%;
      height: 220px;
      position: relative;
    }
    canvas {
      width: 100%;
      height: 100%;
      border-radius: 8px;
      background: #090e1a;
      display: block;
    }

    /* Stat Cards */
    .cards {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 10px;
    }
    @media (max-width: 600px) {
      .cards {
        grid-template-columns: 1fr;
      }
    }
    .stat-card {
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 10px;
      padding: 12px 14px;
      display: flex;
      flex-direction: column;
      gap: 4px;
    }
    .card-val {
      font-size: 1.65rem;
      font-weight: 800;
      line-height: 1.15;
    }
    .card-sub {
      font-size: 0.76rem;
      color: var(--dim);
    }

    /* VU Meter */
    .vu-bar-wrap {
      display: flex;
      flex-direction: column;
      gap: 6px;
      margin-top: 4px;
    }
    .vu-row {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 0.76rem;
    }
    .vu-label {
      width: 24px;
      font-weight: 700;
    }
    .vu-track {
      flex: 1;
      height: 7px;
      background: #1e293b;
      border-radius: 4px;
      overflow: hidden;
      position: relative;
    }
    .vu-fill {
      height: 100%;
      width: 0%;
      background: linear-gradient(90deg, var(--green) 0%, var(--warn) 65%, var(--danger) 90%);
      transition: width 0.08s ease-out;
      border-radius: 4px;
    }
    .vu-thresh-line {
      position: absolute;
      top: 0;
      bottom: 0;
      width: 2px;
      background: #ffffff;
    }
    .vu-val {
      width: 50px;
      text-align: right;
      font-family: monospace;
      font-weight: 700;
    }

    /* Buttons & Controls */
    .btn-row {
      display: flex;
      gap: 6px;
      align-items: center;
      flex-wrap: wrap;
    }
    .btn {
      background: #162033;
      border: 1px solid #334155;
      color: var(--text);
      padding: 5px 10px;
      border-radius: 6px;
      font-size: 0.75rem;
      font-weight: 600;
      cursor: pointer;
      text-decoration: none;
      display: inline-flex;
      align-items: center;
      gap: 4px;
      transition: border-color 0.15s, background 0.15s;
    }
    .btn:hover {
      background: #1e293b;
      border-color: var(--accent);
    }
    .btn-danger:hover {
      background: rgba(255, 0, 85, 0.15);
      border-color: var(--danger);
      color: var(--danger);
    }

    /* Evidence Grid */
    .evidence-grid {
      display: flex;
      gap: 10px;
      flex-wrap: wrap;
      align-items: stretch;
    }
    .photo-box {
      flex: 2;
      min-width: 280px;
      min-height: 240px;
      max-height: 440px;
      background: #070b14;
      border-radius: 8px;
      border: 1px dashed #1e293b;
      display: flex;
      align-items: center;
      justify-content: center;
      overflow: hidden;
      position: relative;
    }
    .crop-box {
      flex: 1;
      min-width: 220px;
      min-height: 240px;
      max-height: 440px;
      background: #070b14;
      border-radius: 8px;
      border: 1px solid #1e293b;
      display: none;
      flex-direction: column;
      justify-content: space-between;
      padding: 10px;
      gap: 8px;
    }

    /* Settings & Telegram Grid */
    .settings-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(290px, 1fr));
      gap: 12px;
    }
    .slider-group {
      display: flex;
      flex-direction: column;
      gap: 6px;
    }
    .slider-header {
      display: flex;
      justify-content: space-between;
      font-size: 0.82rem;
      font-weight: 600;
    }
    .slider-header span { color: var(--dim); }
    .slider-header b { color: var(--accent); font-family: monospace; }
    .slider-ctrl {
      display: flex;
      align-items: center;
      gap: 8px;
    }
    .btn-step {
      background: #162033;
      color: var(--text);
      border: 1px solid #334155;
      border-radius: 6px;
      width: 28px;
      height: 28px;
      font-size: 1rem;
      font-weight: 700;
      cursor: pointer;
    }
    .btn-step:hover {
      border-color: var(--accent);
      color: var(--accent);
    }
    input[type=range] {
      flex: 1;
      height: 6px;
      accent-color: var(--accent);
      cursor: pointer;
    }
    .field-row {
      display: flex;
      flex-direction: column;
      gap: 4px;
    }
    .field-row label {
      font-size: 0.76rem;
      color: var(--dim);
      font-weight: 600;
    }
    .text-input {
      width: 100%;
      background: #080d17;
      border: 1px solid #334155;
      border-radius: 6px;
      padding: 7px 10px;
      color: var(--text);
      font-size: 0.8rem;
      font-family: monospace;
      outline: none;
    }
    .text-input:focus {
      border-color: var(--accent);
    }
    .check-row {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 8px;
      flex-wrap: wrap;
      font-size: 0.8rem;
    }
    .check-label {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      cursor: pointer;
      color: var(--text);
    }
    .tg-status {
      font-size: 0.75rem;
      color: var(--dim);
      font-family: monospace;
      min-height: 18px;
    }

    /* History Table */
    .table-wrap { overflow-x: auto; }
    table {
      width: 100%;
      border-collapse: collapse;
      font-size: 0.8rem;
      text-align: left;
    }
    th {
      color: var(--dim);
      font-weight: 600;
      padding: 8px 10px;
      border-bottom: 1px solid #1e293b;
    }
    td {
      padding: 8px 10px;
      border-bottom: 1px solid #131d31;
      font-family: monospace;
    }
    tr:last-child td { border-bottom: none; }
    .lane-pill {
      display: inline-block;
      padding: 2px 8px;
      border-radius: 4px;
      font-size: 0.72rem;
      font-weight: 700;
      font-family: -apple-system, sans-serif;
    }

    .footer {
      text-align: center;
      font-size: 0.75rem;
      color: var(--dim);
      padding: 4px 0 12px;
    }
  </style>
</head>
<body>
  <div class="container">
    <!-- Header -->
    <div class="header">
      <div class="title">
        <h1><span>ESP32-S3</span> ĐỊNH VỊ TIẾNG ỒN LÀN ĐƯỜNG</h1>
      </div>
      <div class="header-right">
        <div class="badge">
          <div id="connDot" class="dot"></div>
          <span id="connText">Đã kết nối</span>
        </div>
      </div>
    </div>

    <!-- Thanh chuyển Tab -->
    <div class="tabs-bar">
      <button class="tab-btn active" id="btnTabLive" onclick="switchTab('live')">📊 Giám sát trực tiếp</button>
      <button class="tab-btn" id="btnTabHistory" onclick="switchTab('history')">
        📋 Lịch sử vi phạm <span id="historyCountBadge" class="tab-count">0</span>
      </button>
    </div>

    <!-- TAB 1: GIÁM SÁT TRỰC TIẾP -->
    <div id="tabLive" class="tab-pane active">
      <!-- Banner Cảnh Báo -->
      <div id="alertBanner" class="alert-banner">
        <span id="alertMsg">⚠️ PHÁT HIỆN TIẾNG ỒN VƯỢT NGƯỠNG!</span>
        <span id="alertLane" class="lane-pill" style="background: var(--danger); color: #fff;">LÀN TRÁI</span>
      </div>

      <!-- Mô Phỏng Lòng Đường 2D -->
      <div class="card">
        <div class="section-head">
          <span class="card-title">Mô phỏng lòng đường (3 làn)</span>
          <span id="roadDimLabel" style="font-size: 0.78rem; color: var(--dim);">Bề rộng: 30 cm</span>
        </div>
        <div class="road-canvas-wrap">
          <canvas id="roadCanvas"></canvas>
        </div>
      </div>

      <!-- Thông Số Chính -->
      <div class="cards">
        <div class="stat-card">
          <div class="card-title">Độ ồn vi phạm</div>
          <div class="card-val" id="eventDb" style="color: var(--danger);">-- <span style="font-size: 0.9rem; color: var(--dim);">dB</span></div>
          <div class="card-sub" id="eventStatus">Chờ vượt ngưỡng...</div>
        </div>

        <div class="stat-card">
          <div class="card-title">Micro trực tiếp</div>
          <div class="vu-bar-wrap">
            <div class="vu-row">
              <span class="vu-label" style="color: var(--green);">M1</span>
              <div class="vu-track">
                <div id="m1Bar" class="vu-fill"></div>
                <div id="m1Thresh" class="vu-thresh-line" style="left: 62%;"></div>
              </div>
              <span id="m1Db" class="vu-val">-- dB</span>
            </div>
            <div class="vu-row">
              <span class="vu-label" style="color: var(--accent);">M2</span>
              <div class="vu-track">
                <div id="m2Bar" class="vu-fill"></div>
                <div id="m2Thresh" class="vu-thresh-line" style="left: 62%;"></div>
              </div>
              <span id="m2Db" class="vu-val">-- dB</span>
            </div>
          </div>
        </div>
      </div>

      <!-- Bằng Chứng Vi Phạm -->
      <div class="card">
        <div class="section-head">
          <div class="card-title">Bằng chứng vi phạm (Ảnh & Âm thanh 3s)</div>
          <div class="btn-row">
            <span id="photoTimeTag" style="font-size: 0.74rem; color: var(--dim); font-family: monospace;">Chưa có bằng chứng</span>
            <button class="btn" onclick="triggerTestCapture()" style="color: var(--accent);">📸 Chụp thử</button>
            <button id="btnToggleROI" class="btn" onclick="toggleLaneROI()" style="color: var(--green);">📐 3 Làn: BẬT</button>
            <button class="btn" onclick="cycleRotation()">🔄 <span id="rotDeg">270°</span></button>
            <button id="btnDownloadPhotoDirect" class="btn" onclick="downloadRotatedPhoto()" style="display:none; color: var(--accent);">⬇ JPG</button>
            <button id="btnDownloadPhoto" class="btn" onclick="downloadStampedPhoto()" style="display:none; color: var(--warn);">⬇ OSD</button>
            <a id="btnDownloadAudio" href="/audio.wav" download="evidence_3s.wav" class="btn" style="display:none; color: var(--green);">⬇ WAV</a>
          </div>
        </div>

        <div class="evidence-grid">
          <div class="photo-box">
            <img id="camPhoto" src="" alt="Chưa có ảnh chụp" style="display: none;">
            <canvas id="camCanvas" style="display: none; max-width: 100%; max-height: 420px; border-radius: 6px; object-fit: contain;"></canvas>

            <div id="photoWatermark" style="display: none; position: absolute; bottom: 8px; left: 8px; background: rgba(7, 11, 20, 0.85); border-left: 3px solid var(--accent); padding: 4px 8px; border-radius: 4px; font-family: monospace; line-height: 1.3; pointer-events: none; z-index: 5;">
              <div id="wmDateTime" style="font-weight: 700; color: #00f0ff; font-size: 0.76rem;">--</div>
              <div id="wmEventInfo" style="font-size: 0.72rem; color: #ffb703;">--</div>
            </div>

            <span id="camPhotoPlaceholder" style="color: var(--dim); font-size: 0.84rem;">Tự động chụp ảnh và thu âm 3s khi vượt ngưỡng</span>
          </div>

          <div id="plateBoxWrap" class="crop-box">
            <div class="section-head">
              <span style="font-size: 0.74rem; font-weight: 700; color: #ff0055;">🔍 CẬN CẢNH ĐUÔI XE</span>
              <span id="cropLaneBadge" style="font-size: 0.7rem; padding: 2px 6px; border-radius: 4px; font-weight: 700;">TRỐNG</span>
            </div>
            <div style="width: 100%; flex: 1; min-height: 170px; display: flex; align-items: center; justify-content: center; overflow: hidden; border-radius: 6px; border: 1px solid #ff0055; background: #000;">
              <canvas id="plateCropCanvas" style="width: 100%; height: 100%; object-fit: contain;"></canvas>
            </div>
            <div class="section-head">
              <span style="font-size: 0.7rem; color: var(--dim);">Vùng làn vi phạm</span>
              <button class="btn btn-danger" onclick="downloadPlateCrop()" style="font-size: 0.72rem;">⬇ Tải crop</button>
            </div>
          </div>
        </div>

        <div id="audioSection" style="display: none; flex-direction: column; gap: 6px;">
          <div class="section-head" style="font-size: 0.78rem;">
            <span style="color: var(--dim);">🔊 Ghi âm bằng chứng 3 giây (1.5s trước + 1.5s sau)</span>
            <span id="audioStatus" style="color: var(--warn); font-weight: 600;">Đang thu...</span>
          </div>
          <audio id="audioPlayer" controls style="width: 100%; height: 36px;"></audio>
        </div>
      </div>

      <!-- Cài Đặt Cảm Biến & Telegram -->
      <div class="settings-grid">
        <div class="card">
          <div class="card-title">Cấu hình cảm biến</div>
          <div class="slider-group">
            <div class="slider-header">
              <span>Ngưỡng bắt tiếng ồn</span>
              <b id="threshVal">75 dB</b>
            </div>
            <div class="slider-ctrl">
              <button class="btn-step" onclick="stepThreshold(-5)">-</button>
              <input type="range" id="threshSlider" min="40" max="120" value="75" autocomplete="off" oninput="onThresholdInput()">
              <button class="btn-step" onclick="stepThreshold(5)">+</button>
            </div>
          </div>
          <div class="slider-group">
            <div class="slider-header">
              <span>Bề rộng mô hình đường</span>
              <b id="widthVal">30 cm</b>
            </div>
            <div class="slider-ctrl">
              <button class="btn-step" onclick="stepWidth(-5)">-</button>
              <input type="range" id="widthSlider" min="10" max="100" value="30" autocomplete="off" oninput="onWidthInput()">
              <button class="btn-step" onclick="stepWidth(5)">+</button>
            </div>
          </div>
        </div>

        <div class="card">
          <div class="section-head">
            <div class="card-title">Cảnh báo Telegram Bot</div>
            <button class="btn" onclick="sendEvidenceToTelegram(true)" style="color: var(--accent);">📤 Gửi thử</button>
          </div>
          <div class="field-row">
            <label for="tgBotToken">Bot Token</label>
            <input type="password" id="tgBotToken" class="text-input" placeholder="123456789:ABCdef..." oninput="saveTelegramConfig()">
          </div>
          <div class="field-row">
            <label for="tgChatId">Chat ID</label>
            <input type="text" id="tgChatId" class="text-input" placeholder="-1001234567890 hoặc ID cá nhân" oninput="saveTelegramConfig()">
          </div>
          <div class="check-row">
            <label class="check-label">
              <input type="checkbox" id="tgAutoSend" onchange="saveTelegramConfig()">
              <span>Tự động gửi ảnh & WAV khi vi phạm</span>
            </label>
          </div>
          <div id="tgStatusText" class="tg-status">Chưa cấu hình Telegram Bot</div>
        </div>
      </div>
    </div>

    <!-- TAB 2: LỊCH SỬ VI PHẠM -->
    <div id="tabHistory" class="tab-pane">
      <div class="card">
        <div class="section-head">
          <span class="card-title">Lịch sử phát hiện vi phạm tiếng ồn</span>
          <button class="btn btn-danger" onclick="clearHistory()">🗑️ Xóa lịch sử</button>
        </div>
        <div class="table-wrap">
          <table id="historyTable">
            <thead>
              <tr>
                <th>Thời gian</th>
                <th>Độ ồn (dB)</th>
                <th>Vị trí (cm)</th>
                <th>Tỉ lệ (%)</th>
                <th>Độ lệch Δt</th>
                <th>Phân làn</th>
                <th>Telegram</th>
              </tr>
            </thead>
            <tbody id="historyBody">
              <tr id="emptyRow">
                <td colspan="7" style="text-align: center; color: var(--dim); padding: 20px;">Chưa có sự kiện nào được ghi nhận.</td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>
    </div>

    <div class="footer">
      ESP32-S3 CAM • 2x INMP441 I2S 48kHz • TDoA Cross-Correlation
    </div>
  </div>

  <script>
    const canvas = document.getElementById('roadCanvas');
    const ctx = canvas.getContext('2d');

    const connDot = document.getElementById('connDot');
    const connText = document.getElementById('connText');

    const alertBanner = document.getElementById('alertBanner');
    const alertMsg = document.getElementById('alertMsg');
    const alertLane = document.getElementById('alertLane');

    const eventDbEl = document.getElementById('eventDb');
    const eventStatusEl = document.getElementById('eventStatus');

    const m1DbEl = document.getElementById('m1Db');
    const m2DbEl = document.getElementById('m2Db');
    const m1Bar = document.getElementById('m1Bar');
    const m2Bar = document.getElementById('m2Bar');
    const m1Thresh = document.getElementById('m1Thresh');
    const m2Thresh = document.getElementById('m2Thresh');

    const threshSlider = document.getElementById('threshSlider');
    const widthSlider = document.getElementById('widthSlider');
    const threshVal = document.getElementById('threshVal');
    const widthVal = document.getElementById('widthVal');
    const roadDimLabel = document.getElementById('roadDimLabel');

    const historyBody = document.getElementById('historyBody');
    const historyCountBadge = document.getElementById('historyCountBadge');

    const tgBotTokenEl = document.getElementById('tgBotToken');
    const tgChatIdEl = document.getElementById('tgChatId');
    const tgAutoSendEl = document.getElementById('tgAutoSend');
    const tgStatusText = document.getElementById('tgStatusText');

    let roadWidth = parseFloat(localStorage.getItem('cfg_width') || '30');
    let threshold = parseFloat(localStorage.getItem('cfg_thresh') || '75');

    if (threshSlider) threshSlider.value = threshold;
    if (widthSlider) widthSlider.value = roadWidth;
    if (threshVal) threshVal.textContent = threshold + ' dB';
    if (widthVal) widthVal.textContent = roadWidth + ' cm';

    let targetCarPosPct = 50;
    let currentCarPosPct = 50;
    let carOpacity = 0.3;
    let pulseRadius = 0;
    let lastEventTime = 0;
    let lastEventData = null;
    let totalHistoryCount = 0;
    let lastHistoryTgCell = null;

    // Trạng thái đồng bộ gửi bằng chứng Telegram khi có vi phạm
    let pendingTgPhotoReady = false;
    let pendingTgAudioReady = false;
    let pendingTgSent = false;

    function switchTab(tabName) {
      const isLive = (tabName === 'live');
      document.getElementById('tabLive').classList.toggle('active', isLive);
      document.getElementById('tabHistory').classList.toggle('active', !isLive);
      document.getElementById('btnTabLive').classList.toggle('active', isLive);
      document.getElementById('btnTabHistory').classList.toggle('active', !isLive);
      if (isLive) {
        requestAnimationFrame(() => resizeCanvas());
      }
    }

    function loadTelegramConfig() {
      const savedToken = localStorage.getItem('cfg_tg_token') || '';
      const savedChatId = localStorage.getItem('cfg_tg_chat_id') || '';
      const savedAuto = localStorage.getItem('cfg_tg_auto');
      if (tgBotTokenEl) tgBotTokenEl.value = savedToken;
      if (tgChatIdEl) tgChatIdEl.value = savedChatId;
      if (tgAutoSendEl) tgAutoSendEl.checked = (savedAuto === null ? true : savedAuto === 'true');
      updateTgStatusHint();
    }

    function saveTelegramConfig() {
      if (tgBotTokenEl) localStorage.setItem('cfg_tg_token', tgBotTokenEl.value.trim());
      if (tgChatIdEl) localStorage.setItem('cfg_tg_chat_id', tgChatIdEl.value.trim());
      if (tgAutoSendEl) localStorage.setItem('cfg_tg_auto', String(tgAutoSendEl.checked));
      updateTgStatusHint();
    }

    function updateTgStatusHint() {
      if (!tgStatusText) return;
      const token = tgBotTokenEl ? tgBotTokenEl.value.trim() : '';
      const chatId = tgChatIdEl ? tgChatIdEl.value.trim() : '';
      if (!token || !chatId) {
        tgStatusText.textContent = 'Nhập Bot Token và Chat ID để tự động gửi bằng chứng';
        tgStatusText.style.color = 'var(--dim)';
      } else if (tgAutoSendEl && tgAutoSendEl.checked) {
        tgStatusText.textContent = 'Sẵn sàng tự động gửi lên Telegram khi vi phạm';
        tgStatusText.style.color = 'var(--green)';
      } else {
        tgStatusText.textContent = 'Đã lưu cấu hình (Tự động gửi đang tắt)';
        tgStatusText.style.color = 'var(--warn)';
      }
    }

    function resizeCanvas() {
      const dpr = window.devicePixelRatio || 1;
      const rect = canvas.getBoundingClientRect();
      if (rect.width <= 0 || rect.height <= 0) return;
      canvas.width = rect.width * dpr;
      canvas.height = rect.height * dpr;
      ctx.resetTransform ? ctx.resetTransform() : ctx.setTransform(1, 0, 0, 1, 0, 0);
      ctx.scale(dpr, dpr);
    }
    window.addEventListener('resize', resizeCanvas);
    resizeCanvas();

    function drawRoad() {
      const rect = canvas.getBoundingClientRect();
      const w = rect.width;
      const h = rect.height;
      if (w <= 0 || h <= 0) {
        requestAnimationFrame(drawRoad);
        return;
      }
      ctx.clearRect(0, 0, w, h);

      const marginX = 70;
      const roadTop = 32;
      const roadH = h - 64;
      const roadW = w - marginX * 2;
      const centerY = roadTop + roadH / 2;

      // 1. Mặt đường chính
      ctx.fillStyle = '#111827';
      ctx.fillRect(marginX, roadTop, roadW, roadH);

      // 2. Viền mép đường
      ctx.fillStyle = '#f8fafc';
      ctx.fillRect(marginX, roadTop, roadW, 4);
      ctx.fillRect(marginX, roadTop + roadH - 4, roadW, 4);

      // 3. Hai vạch phân làn
      const xLane1 = marginX + roadW * 0.35;
      const xLane2 = marginX + roadW * 0.68;

      ctx.strokeStyle = '#ffb703';
      ctx.lineWidth = 2.5;
      ctx.setLineDash([12, 10]);

      ctx.beginPath();
      ctx.moveTo(xLane1, roadTop);
      ctx.lineTo(xLane1, roadTop + roadH);
      ctx.stroke();

      ctx.beginPath();
      ctx.moveTo(xLane2, roadTop);
      ctx.lineTo(xLane2, roadTop + roadH);
      ctx.stroke();
      ctx.setLineDash([]);

      // Nhãn 3 làn đường
      ctx.fillStyle = 'rgba(148, 163, 184, 0.4)';
      ctx.font = 'bold 11px -apple-system, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('LÀN 1 (TRÁI)', marginX + roadW * 0.175, roadTop + 20);
      ctx.fillText('LÀN 2 (GIỮA)', marginX + roadW * 0.515, roadTop + 20);
      ctx.fillText('LÀN 3 (PHẢI)', marginX + roadW * 0.84, roadTop + 20);

      // Thước đo cm
      ctx.fillStyle = '#64748b';
      ctx.font = '9px monospace';
      const steps = 4;
      for (let s = 0; s <= steps; s++) {
        const xTick = marginX + (s / steps) * roadW;
        const cmVal = ((s / steps) * roadWidth).toFixed(0);
        ctx.fillRect(xTick - 0.5, roadTop - 7, 1, 5);
        ctx.fillText(`${cmVal}cm`, xTick, roadTop - 11);
      }

      // 4. Hai trạm Micro
      ctx.fillStyle = '#00ff88';
      ctx.beginPath();
      ctx.arc(marginX, centerY, 9, 0, Math.PI * 2);
      ctx.fill();

      ctx.font = 'bold 11px -apple-system, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('MIC 1', marginX - 36, centerY - 4);
      ctx.font = '10px -apple-system, sans-serif';
      ctx.fillStyle = '#94a3b8';
      ctx.fillText('(0 cm)', marginX - 36, centerY + 11);

      ctx.fillStyle = '#00f0ff';
      ctx.beginPath();
      ctx.arc(marginX + roadW, centerY, 9, 0, Math.PI * 2);
      ctx.fill();

      ctx.font = 'bold 11px -apple-system, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('MIC 2', marginX + roadW + 36, centerY - 4);
      ctx.font = '10px -apple-system, sans-serif';
      ctx.fillStyle = '#94a3b8';
      ctx.fillText(`(${roadWidth} cm)`, marginX + roadW + 36, centerY + 11);

      // 5. Nội suy vị trí xe
      currentCarPosPct += (targetCarPosPct - currentCarPosPct) * 0.16;
      const carX = marginX + (currentCarPosPct / 100.0) * roadW;

      // 6. Sóng lan tỏa
      if (pulseRadius > 0 && pulseRadius < 110) {
        pulseRadius += 2.8;
        ctx.strokeStyle = `rgba(255, 0, 85, ${Math.max(0, 1.0 - pulseRadius / 110)})`;
        ctx.lineWidth = 2.5;
        ctx.beginPath();
        ctx.arc(carX, centerY, pulseRadius, 0, Math.PI * 2);
        ctx.stroke();

        ctx.strokeStyle = `rgba(255, 183, 3, ${Math.max(0, 0.8 - pulseRadius / 110)})`;
        ctx.lineWidth = 1.5;
        ctx.beginPath();
        ctx.arc(carX, centerY, Math.max(0, pulseRadius - 15), 0, Math.PI * 2);
        ctx.stroke();
      }

      // 7. Xe quay đầu hướng lên trên (-Y)
      ctx.save();
      ctx.translate(carX, centerY);

      // Chùm sáng đèn pha hướng lên trên
      const beamAlpha = carOpacity > 0.6 ? 0.25 : 0.08;
      ctx.fillStyle = `rgba(255, 183, 3, ${beamAlpha})`;
      ctx.beginPath();
      ctx.moveTo(-9, -21);
      ctx.lineTo(-16, -38);
      ctx.lineTo(-3, -38);
      ctx.closePath();
      ctx.fill();

      ctx.beginPath();
      ctx.moveTo(9, -21);
      ctx.lineTo(3, -38);
      ctx.lineTo(16, -38);
      ctx.closePath();
      ctx.fill();

      // Thân xe dọc (26x40)
      ctx.fillStyle = `rgba(255, 0, 85, ${carOpacity})`;
      ctx.shadowColor = '#ff0055';
      ctx.shadowBlur = carOpacity > 0.6 ? 20 : 6;
      ctx.beginPath();
      ctx.roundRect ? ctx.roundRect(-13, -20, 26, 40, 6) : ctx.rect(-13, -20, 26, 40);
      ctx.fill();
      ctx.shadowBlur = 0;

      // Kính chắn gió phía trên (đầu xe) & kính sau phía dưới
      ctx.fillStyle = '#e2e8f0';
      ctx.fillRect(-10, -12, 20, 9);
      ctx.fillStyle = 'rgba(226, 232, 240, 0.7)';
      ctx.fillRect(-9, 8, 18, 6);

      // 2 đèn pha vàng phía trước (trên)
      ctx.fillStyle = '#ffb703';
      ctx.fillRect(-10, -21, 5, 3);
      ctx.fillRect(5, -21, 5, 3);

      // 2 đèn hậu đỏ phía sau (dưới)
      ctx.fillStyle = '#ff3366';
      ctx.fillRect(-10, 18, 5, 3);
      ctx.fillRect(5, 18, 5, 3);
      ctx.restore();

      // Tag khoảng cách phía dưới mặt đường để không đè thước đo phía trên
      ctx.fillStyle = carOpacity > 0.6 ? '#ffffff' : '#94a3b8';
      ctx.font = 'bold 11px monospace';
      ctx.textAlign = 'center';
      const distFromLeft = ((currentCarPosPct / 100.0) * roadWidth).toFixed(1);
      ctx.fillText(`${distFromLeft} cm (${currentCarPosPct.toFixed(0)}%)`, carX, roadTop + roadH + 18);

      // Đường gióng đứt nét
      ctx.strokeStyle = `rgba(255, 0, 85, ${carOpacity > 0.6 ? 0.55 : 0.2})`;
      ctx.lineWidth = 1.5;
      ctx.setLineDash([4, 4]);
      ctx.beginPath();
      ctx.moveTo(carX, roadTop);
      ctx.lineTo(carX, roadTop + roadH);
      ctx.stroke();
      ctx.setLineDash([]);

      if (Date.now() - lastEventTime > 5000) {
        carOpacity = Math.max(0.28, carOpacity - 0.015);
        alertBanner.style.display = 'none';
      }

      requestAnimationFrame(drawRoad);
    }
    requestAnimationFrame(drawRoad);

    // Đồng bộ cài đặt cảm biến qua WebSocket
    let sendTimeout = null;
    function syncSettingsToWs() {
      if (threshSlider) threshSlider.value = threshold;
      if (widthSlider) widthSlider.value = roadWidth;
      if (threshVal) threshVal.textContent = threshold + ' dB';
      if (widthVal) widthVal.textContent = roadWidth + ' cm';
      if (roadDimLabel) roadDimLabel.textContent = `Bề rộng: ${roadWidth} cm`;

      const threshPct = Math.min(100, Math.max(0, (threshold / 120) * 100));
      if (m1Thresh) m1Thresh.style.left = threshPct + '%';
      if (m2Thresh) m2Thresh.style.left = threshPct + '%';

      clearTimeout(sendTimeout);
      sendTimeout = setTimeout(() => {
        if (ws && ws.readyState === WebSocket.OPEN) {
          ws.send(JSON.stringify({ cmd: "set", width: roadWidth, thresh: threshold }));
        }
      }, 150);
    }

    function onThresholdInput() {
      threshold = parseFloat(threshSlider.value);
      localStorage.setItem('cfg_thresh', threshold);
      syncSettingsToWs();
    }

    function onWidthInput() {
      roadWidth = parseFloat(widthSlider.value);
      localStorage.setItem('cfg_width', roadWidth);
      syncSettingsToWs();
    }

    function stepThreshold(delta) {
      threshold = Math.min(120, Math.max(40, threshold + delta));
      localStorage.setItem('cfg_thresh', threshold);
      syncSettingsToWs();
    }

    function stepWidth(delta) {
      roadWidth = Math.min(100, Math.max(10, roadWidth + delta));
      localStorage.setItem('cfg_width', roadWidth);
      syncSettingsToWs();
    }

    function updateHistoryBadge() {
      if (historyCountBadge) {
        historyCountBadge.textContent = String(totalHistoryCount);
      }
    }

    function addHistoryRow(ev) {
      const empty = document.getElementById('emptyRow');
      if (empty) empty.remove();

      const timeStr = new Date().toLocaleTimeString('vi-VN');
      const laneId = ev.lane || (ev.pct < LANE_1_MAX_PCT ? 1 : (ev.pct > LANE_2_MAX_PCT ? 3 : 2));
      let laneName = "LÀN 2 (GIỮA)";
      let laneBg = "var(--warn)";
      if (laneId === 1) {
        laneName = "LÀN 1 (TRÁI)";
        laneBg = "var(--green)";
      } else if (laneId === 3) {
        laneName = "LÀN 3 (PHẢI)";
        laneBg = "var(--accent)";
      }

      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td style="color: var(--dim);">${timeStr}</td>
        <td style="color: var(--danger); font-weight:700;">${ev.db.toFixed(1)} dB</td>
        <td style="color: var(--accent); font-weight:700;">${ev.pos.toFixed(1)} cm</td>
        <td>${ev.pct.toFixed(1)}%</td>
        <td style="color: var(--warn);">${(ev.tdoa > 0 ? '+' : '') + ev.tdoa.toFixed(0)} μs</td>
        <td><span class="lane-pill" style="background:${laneBg}; color:#000;">${laneName}</span></td>
        <td class="tg-cell" style="color: var(--dim);">Đang chờ...</td>
      `;

      lastHistoryTgCell = tr.querySelector('.tg-cell');
      historyBody.insertBefore(tr, historyBody.firstChild);

      totalHistoryCount++;
      updateHistoryBadge();

      while (historyBody.children.length > 50) {
        historyBody.removeChild(historyBody.lastChild);
      }
    }

    function clearHistory() {
      totalHistoryCount = 0;
      lastHistoryTgCell = null;
      updateHistoryBadge();
      historyBody.innerHTML = `
        <tr id="emptyRow">
          <td colspan="7" style="text-align: center; color: var(--dim); padding: 20px;">Chưa có sự kiện nào được ghi nhận.</td>
        </tr>
      `;
    }

    const LANE_1_MAX_PCT = 38.5;
    const LANE_2_MAX_PCT = 74.0;
    let showLaneROI = (localStorage.getItem('show_lane_roi') !== 'false');
    let activeViolationLane = 0;
    let currentRotation = parseInt(localStorage.getItem('cam_rot') || '270');
    let lastDetectedVBox = null;

    function updateROIBtnLabel() {
      const el = document.getElementById('btnToggleROI');
      if (el) {
        el.textContent = '📐 3 Làn: ' + (showLaneROI ? 'BẬT' : 'TẮT');
        el.style.color = showLaneROI ? 'var(--green)' : 'var(--dim)';
        el.style.borderColor = showLaneROI ? 'rgba(0,255,136,0.4)' : 'var(--border)';
      }
    }

    function toggleLaneROI() {
      showLaneROI = !showLaneROI;
      localStorage.setItem('show_lane_roi', showLaneROI);
      updateROIBtnLabel();
      renderPhotoToCanvas();
    }

    function findVehicleBoundingBox(ctx, w, h, targetLane) {
      if (targetLane < 1 || targetLane > 3) return null;

      let scanX1, scanX2;
      if (targetLane === 1) {
        scanX1 = 28; scanX2 = 160;
      } else if (targetLane === 2) {
        scanX1 = 212; scanX2 = 328;
      } else {
        scanX1 = 380; scanX2 = 465;
      }

      const scanY1 = 255;
      const scanY2 = 615;
      const sw = scanX2 - scanX1;
      const sh = scanY2 - scanY1;
      if (sw <= 15 || sh <= 20) return null;

      try {
        const imgData = ctx.getImageData(scanX1, scanY1, sw, sh);
        const data = imgData.data;
        const rawMask = new Uint8Array(sw * sh);

        for (let y = 0; y < sh; y += 2) {
          for (let x = 0; x < sw; x += 2) {
            const idx = (y * sw + x) * 4;
            const r = data[idx];
            const g = data[idx + 1];
            const b = data[idx + 2];

            const maxC = Math.max(r, g, b);
            const minC = Math.min(r, g, b);
            const diffC = maxC - minC;
            const sat = maxC > 0 ? diffC / maxC : 0;

            const isWhiteRoad = (minC > 180 && sat < 0.10);
            const isNeutralShadow = (minC > 115 && diffC < 20 && sat < 0.11);

            const isColorCar = (diffC >= 18 && sat >= 0.13);
            const isRedDominant = (r > 75 && (r - g >= 14) && (r - b >= 14));
            const isDarkCar = (maxC < 125 && diffC >= 8);

            if (!isWhiteRoad && !isNeutralShadow && (isColorCar || isRedDominant || isDarkCar)) {
              rawMask[y * sw + x] = 1;
              if (x + 1 < sw) rawMask[y * sw + x + 1] = 1;
              if (y + 1 < sh) {
                rawMask[(y + 1) * sw + x] = 1;
                if (x + 1 < sw) rawMask[(y + 1) * sw + x + 1] = 1;
              }
            }
          }
        }

        let curMask = rawMask;
        for (let pass = 0; pass < 2; pass++) {
          const nextMask = new Uint8Array(sw * sh);
          for (let y = 1; y < sh - 1; y++) {
            for (let x = 1; x < sw - 1; x++) {
              const idx = y * sw + x;
              if (curMask[idx] && curMask[idx - 1] && curMask[idx + 1] &&
                  curMask[idx - sw] && curMask[idx + sw]) {
                nextMask[idx] = 1;
              }
            }
          }
          curMask = nextMask;
        }

        const dilMask = new Uint8Array(sw * sh);
        const rad = 2;
        for (let y = rad; y < sh - rad; y++) {
          for (let x = rad; x < sw - rad; x++) {
            if (curMask[y * sw + x]) {
              for (let dy = -rad; dy <= rad; dy++) {
                for (let dx = -rad; dx <= rad; dx++) {
                  dilMask[(y + dy) * sw + (x + dx)] = 1;
                }
              }
            }
          }
        }

        const exterior = new Uint8Array(sw * sh);
        const queueX = [];
        const queueY = [];

        for (let x = 0; x < sw; x++) {
          if (!dilMask[x]) { exterior[x] = 1; queueX.push(x); queueY.push(0); }
          const bIdx = (sh - 1) * sw + x;
          if (!dilMask[bIdx] && !exterior[bIdx]) { exterior[bIdx] = 1; queueX.push(x); queueY.push(sh - 1); }
        }
        for (let y = 0; y < sh; y++) {
          const lIdx = y * sw;
          if (!dilMask[lIdx] && !exterior[lIdx]) { exterior[lIdx] = 1; queueX.push(0); queueY.push(y); }
          const rIdx = y * sw + (sw - 1);
          if (!dilMask[rIdx] && !exterior[rIdx]) { exterior[rIdx] = 1; queueX.push(sw - 1); queueY.push(y); }
        }

        let qHead = 0;
        while (qHead < queueX.length) {
          const cx = queueX[qHead];
          const cy = queueY[qHead++];
          const neighbors = [[cx - 1, cy], [cx + 1, cy], [cx, cy - 1], [cx, cy + 1]];
          for (let i = 0; i < 4; i++) {
            const nx = neighbors[i][0];
            const ny = neighbors[i][1];
            if (nx >= 0 && nx < sw && ny >= 0 && ny < sh) {
              const nIdx = ny * sw + nx;
              if (!exterior[nIdx] && !dilMask[nIdx]) {
                exterior[nIdx] = 1;
                queueX.push(nx);
                queueY.push(ny);
              }
            }
          }
        }

        const cleanMask = new Uint8Array(sw * sh);
        for (let i = 0; i < sw * sh; i++) {
          cleanMask[i] = exterior[i] ? 0 : 1;
        }

        const rowSums = new Array(sh).fill(0);
        for (let y = 0; y < sh; y += 2) {
          for (let x = 0; x < sw; x += 2) {
            if (cleanMask[y * sw + x]) rowSums[y]++;
          }
        }

        let bestCluster = null;
        let curStart = -1, curEnd = -1, curDense = 0, gap = 0;

        for (let y = 0; y < sh; y += 2) {
          if (rowSums[y] >= 8) {
            if (curStart === -1) curStart = y;
            curEnd = y;
            curDense++;
            gap = 0;
          } else if (curStart !== -1) {
            gap += 2;
            if (gap > 12) {
              const clusterHeight = curEnd - curStart;
              if (curDense >= 10 && clusterHeight >= 30) {
                if (!bestCluster || curDense > bestCluster.dense) {
                  bestCluster = { start: curStart, end: curEnd, dense: curDense };
                }
              }
              curStart = -1; curEnd = -1; curDense = 0; gap = 0;
            }
          }
        }
        if (curStart !== -1 && curDense >= 10 && (curEnd - curStart >= 30)) {
          if (!bestCluster || curDense > bestCluster.dense) {
            bestCluster = { start: curStart, end: curEnd, dense: curDense };
          }
        }

        if (bestCluster) {
          let minX = sw, maxX = 0;
          for (let y = bestCluster.start; y <= bestCluster.end; y += 2) {
            for (let x = 0; x < sw; x += 2) {
              if (cleanMask[y * sw + x]) {
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
              }
            }
          }

          const carW = maxX - minX;
          const carH = bestCluster.end - bestCluster.start;

          if (carW >= 20 && carH >= 25 && (carH / carW <= 3.0)) {
            const padX = 6, padY = 6;
            const bx = Math.max(0, scanX1 + minX - padX);
            const by = Math.max(0, scanY1 + bestCluster.start - padY);
            const bw = Math.min(w - bx, carW + padX * 2);
            const bh = Math.min(h - by, carH + padY * 2);
            return { x: bx, y: by, w: bw, h: bh, lane: targetLane };
          }
        }
      } catch (e) {
        console.warn('Canvas pixel scan fallback:', e);
      }
      return null;
    }

    function drawLaneOverlay(ctx, w, h, activeLane, bottomOffset = 0) {
      if (!showLaneROI) return;

      const x1 = Math.round(w * (LANE_1_MAX_PCT / 100.0));
      const x2 = Math.round(w * (LANE_2_MAX_PCT / 100.0));
      const topY = 0;
      const bottomY = h - bottomOffset;

      ctx.save();
      const hasSoundViolation = (activeLane >= 1 && activeLane <= 3);

      const detectedCars = [];
      for (let l = 1; l <= 3; l++) {
        const vbox = findVehicleBoundingBox(ctx, w, h, l);
        if (vbox) detectedCars.push(vbox);
      }

      let focusCar = null;
      if (hasSoundViolation) {
        const violatorCar = detectedCars.find(c => c.lane === activeLane) || null;
        focusCar = violatorCar || detectedCars[0] || null;
      } else {
        focusCar = detectedCars[0] || null;
      }
      lastDetectedVBox = focusCar;

      if (hasSoundViolation) {
        const rx = (activeLane === 1) ? 0 : (activeLane === 2 ? x1 : x2);
        const rw = (activeLane === 1) ? x1 : (activeLane === 2 ? x2 - x1 : w - x2);
        ctx.fillStyle = 'rgba(255, 0, 85, 0.16)';
        ctx.fillRect(rx, topY, rw, bottomY - topY);
        ctx.strokeStyle = '#ff0055';
        ctx.lineWidth = 2;
        ctx.strokeRect(rx + 1, topY + 1, rw - 2, bottomY - topY - 2);
      }

      detectedCars.forEach(car => {
        const isViolator = (hasSoundViolation && car.lane === activeLane);
        if (isViolator) {
          ctx.strokeStyle = '#ff0055';
          ctx.lineWidth = 3.5;
          ctx.strokeRect(car.x, car.y, car.w, car.h);

          const tagH = 22;
          const tagW = Math.min(180, car.w + 24);
          const tagX = Math.max(4, car.x + (car.w - tagW) / 2);
          const tagY = Math.max(34, car.y - tagH - 4);
          ctx.fillStyle = '#ff0055';
          ctx.fillRect(tagX, tagY, tagW, tagH);
          ctx.fillStyle = '#ffffff';
          ctx.font = 'bold 11px monospace';
          ctx.textAlign = 'center';
          ctx.fillText(`🚨 VI PHẠM (LÀN ${car.lane})`, tagX + tagW / 2, tagY + 15);
        } else {
          ctx.strokeStyle = '#00ff88';
          ctx.lineWidth = 2.5;
          ctx.strokeRect(car.x, car.y, car.w, car.h);
        }
      });

      ctx.strokeStyle = 'rgba(0, 240, 255, 0.7)';
      ctx.lineWidth = 2;
      ctx.setLineDash([8, 6]);
      ctx.beginPath(); ctx.moveTo(x1, topY); ctx.lineTo(x1, bottomY); ctx.stroke();
      ctx.beginPath(); ctx.moveTo(x2, topY); ctx.lineTo(x2, bottomY); ctx.stroke();
      ctx.setLineDash([]);

      const headerH = 22;
      ctx.font = 'bold 11px monospace';
      ctx.textAlign = 'center';
      const lanes = [
        { name: 'LÀN 1', cx: x1 / 2, active: (hasSoundViolation && activeLane === 1), col: '#00ff88' },
        { name: 'LÀN 2', cx: x1 + (x2 - x1) / 2, active: (hasSoundViolation && activeLane === 2), col: '#ffb703' },
        { name: 'LÀN 3', cx: x2 + (w - x2) / 2, active: (hasSoundViolation && activeLane === 3), col: '#00f0ff' }
      ];

      lanes.forEach((lane, idx) => {
        const tagW = Math.min(90, (idx === 0 ? x1 : (idx === 1 ? x2 - x1 : w - x2)) - 8);
        const tagX = lane.cx - tagW / 2;
        const tagY = 6;
        ctx.fillStyle = lane.active ? 'rgba(255, 0, 85, 0.85)' : 'rgba(7, 11, 20, 0.75)';
        ctx.fillRect(tagX, tagY, tagW, headerH);
        ctx.fillStyle = lane.active ? '#ffffff' : lane.col;
        ctx.fillText(lane.active ? `🚨 ${lane.name}` : lane.name, lane.cx, tagY + 15);
      });

      ctx.restore();
    }

    function updateRotBtnLabel() {
      const el = document.getElementById('rotDeg');
      if (el) el.textContent = currentRotation + '°';
    }

    function cycleRotation() {
      const angles = [0, 90, 180, 270];
      const idx = angles.indexOf(currentRotation);
      currentRotation = angles[(idx + 1) % angles.length];
      localStorage.setItem('cam_rot', currentRotation);
      updateRotBtnLabel();
      renderPhotoToCanvas();
    }

    function renderPhotoToCanvas() {
      const camPhoto = document.getElementById('camPhoto');
      const camCanvas = document.getElementById('camCanvas');
      const placeholder = document.getElementById('camPhotoPlaceholder');
      if (!camPhoto || !camPhoto.naturalWidth) return;

      const isRot90 = (currentRotation === 90 || currentRotation === 270);
      camCanvas.width = isRot90 ? camPhoto.naturalHeight : camPhoto.naturalWidth;
      camCanvas.height = isRot90 ? camPhoto.naturalWidth : camPhoto.naturalHeight;
      const cctx = camCanvas.getContext('2d');

      cctx.clearRect(0, 0, camCanvas.width, camCanvas.height);
      cctx.save();
      cctx.translate(camCanvas.width / 2, camCanvas.height / 2);
      cctx.rotate((currentRotation * Math.PI) / 180);
      cctx.drawImage(camPhoto, -camPhoto.naturalWidth / 2, -camPhoto.naturalHeight / 2);
      cctx.restore();

      drawLaneOverlay(cctx, camCanvas.width, camCanvas.height, activeViolationLane, 0);
      renderPlateCrop(camCanvas, lastDetectedVBox);

      camCanvas.style.display = 'block';
      if (placeholder) placeholder.style.display = 'none';
    }

    function renderPlateCrop(mainCanvas, vbox) {
      const plateWrap = document.getElementById('plateBoxWrap');
      const plateCanvas = document.getElementById('plateCropCanvas');
      const badge = document.getElementById('cropLaneBadge');
      if (!plateWrap || !plateCanvas) return;

      plateCanvas.width = 360;
      plateCanvas.height = 270;
      const pctx = plateCanvas.getContext('2d');

      const hasSoundViolation = (activeViolationLane >= 1 && activeViolationLane <= 3);
      const isViolator = (hasSoundViolation && vbox && vbox.lane === activeViolationLane);

      if (!isViolator || !vbox) {
        pctx.clearRect(0, 0, plateCanvas.width, plateCanvas.height);
        pctx.fillStyle = '#070b14';
        pctx.fillRect(0, 0, plateCanvas.width, plateCanvas.height);
        pctx.fillStyle = '#64748b';
        pctx.font = '12px monospace';
        pctx.textAlign = 'center';
        pctx.fillText('(Chỉ cắt khi có xe vi phạm)', plateCanvas.width / 2, plateCanvas.height / 2);

        if (badge) {
          badge.textContent = 'TRỐNG';
          badge.style.color = '#64748b';
          badge.style.background = 'transparent';
        }
        plateWrap.style.display = 'flex';
        return;
      }

      const cropY = Math.round(vbox.y + vbox.h * 0.25);
      const cropH = Math.round(vbox.h * 0.75);
      const cropX = Math.round(vbox.x);
      const cropW = Math.round(vbox.w);

      pctx.clearRect(0, 0, plateCanvas.width, plateCanvas.height);
      pctx.drawImage(mainCanvas, cropX, cropY, cropW, cropH, 0, 0, plateCanvas.width, plateCanvas.height);

      if (badge) {
        badge.textContent = `LÀN ${vbox.lane} (VI PHẠM)`;
        badge.style.color = '#ff0055';
        badge.style.background = 'rgba(255,0,85,0.2)';
      }
      plateWrap.style.display = 'flex';
    }

    function downloadPlateCrop() {
      const pCanvas = document.getElementById('plateCropCanvas');
      if (!pCanvas || !pCanvas.width) return;
      const a = document.createElement('a');
      const dtText = (lastEventData && lastEventData.dateTime) ? lastEventData.dateTime.replace(/[: ]/g, '_') : 'crop';
      a.download = `plate_crop_${dtText}.jpg`;
      a.href = pCanvas.toDataURL('image/jpeg', 0.95);
      a.click();
    }

    function getCleanRotatedCanvas() {
      const camPhoto = document.getElementById('camPhoto');
      if (!camPhoto || !camPhoto.naturalWidth) return null;

      const c = document.createElement('canvas');
      const isRot90 = (currentRotation === 90 || currentRotation === 270);
      c.width = isRot90 ? camPhoto.naturalHeight : camPhoto.naturalWidth;
      c.height = isRot90 ? camPhoto.naturalWidth : camPhoto.naturalHeight;
      const cctx = c.getContext('2d');

      cctx.save();
      cctx.translate(c.width / 2, c.height / 2);
      cctx.rotate((currentRotation * Math.PI) / 180);
      cctx.drawImage(camPhoto, -camPhoto.naturalWidth / 2, -camPhoto.naturalHeight / 2);
      cctx.restore();
      return c;
    }

    function downloadRotatedPhoto() {
      const cleanCanvas = getCleanRotatedCanvas();
      if (!cleanCanvas) return;
      const a = document.createElement('a');
      const dtText = (lastEventData && lastEventData.dateTime) ? lastEventData.dateTime.replace(/[: ]/g, '_') : 'photo';
      a.download = `raw_${dtText}.jpg`;
      a.href = cleanCanvas.toDataURL('image/jpeg', 0.95);
      a.click();
    }

    function createStampedCanvas() {
      const camPhoto = document.getElementById('camPhoto');
      if (!camPhoto || !camPhoto.naturalWidth) return null;

      const c = getCleanRotatedCanvas();
      const cctx = c.getContext('2d');

      const barH = Math.max(52, Math.round(c.height * 0.12));
      drawLaneOverlay(cctx, c.width, c.height, activeViolationLane, barH);

      cctx.fillStyle = 'rgba(0, 0, 0, 0.78)';
      cctx.fillRect(0, c.height - barH, c.width, barH);
      cctx.fillStyle = '#00f0ff';
      cctx.fillRect(0, c.height - barH, c.width, 3);

      const fontSize = Math.max(15, Math.round(barH * 0.36));
      cctx.font = `bold ${fontSize}px monospace`;
      cctx.fillStyle = '#00f0ff';
      const dtText = lastEventData ? lastEventData.dateTime : new Date().toLocaleString('vi-VN');
      cctx.fillText(dtText, 16, c.height - barH * 0.52);

      cctx.font = `${Math.round(fontSize * 0.85)}px monospace`;
      cctx.fillStyle = '#ffb703';
      const infoText = lastEventData ? `${lastEventData.db} dB | ${lastEventData.lane} (${lastEventData.pos} cm)` : '';
      cctx.fillText(infoText, 16, c.height - barH * 0.18);

      return { canvas: c, dtText };
    }

    function createStampedPhotoBlob() {
      return new Promise((resolve) => {
        const stamped = createStampedCanvas();
        if (!stamped) {
          resolve(null);
          return;
        }
        stamped.canvas.toBlob((blob) => resolve(blob), 'image/jpeg', 0.92);
      });
    }

    function downloadStampedPhoto() {
      const stamped = createStampedCanvas();
      if (!stamped) return;
      const a = document.createElement('a');
      a.download = `noise_osd_${stamped.dtText.replace(/[: ]/g, '_')}.jpg`;
      a.href = stamped.canvas.toDataURL('image/jpeg', 0.95);
      a.click();
    }

    function updateHistoryTgStatus(text, color) {
      if (!lastHistoryTgCell) return;
      lastHistoryTgCell.textContent = text;
      lastHistoryTgCell.style.color = color;
    }

    function tryAutoSendTelegram() {
      if (!pendingTgPhotoReady || !pendingTgAudioReady || pendingTgSent) return;
      pendingTgSent = true;
      if (tgAutoSendEl && !tgAutoSendEl.checked) {
        if (tgStatusText) {
          tgStatusText.textContent = 'Tự động gửi đang tắt — bỏ qua gửi Telegram';
          tgStatusText.style.color = 'var(--warn)';
        }
        updateHistoryTgStatus('Đang tắt', 'var(--dim)');
        return;
      }
      sendEvidenceToTelegram(false);
    }

    async function sendEvidenceToTelegram(isManualTest) {
      const token = tgBotTokenEl ? tgBotTokenEl.value.trim() : '';
      const chatId = tgChatIdEl ? tgChatIdEl.value.trim() : '';

      if (!token || !chatId) {
        const msg = 'Thiếu Bot Token hoặc Chat ID';
        if (tgStatusText) {
          tgStatusText.textContent = '⚠️ ' + msg;
          tgStatusText.style.color = 'var(--warn)';
        }
        if (!isManualTest) updateHistoryTgStatus('Thiếu cấu hình', 'var(--warn)');
        return;
      }

      if (tgStatusText) {
        tgStatusText.textContent = '⏳ Đang gửi ảnh & âm thanh lên Telegram...';
        tgStatusText.style.color = 'var(--accent)';
      }
      if (!isManualTest) updateHistoryTgStatus('Đang gửi...', 'var(--accent)');

      const evInfo = lastEventData || {
        dateTime: formatNowDateTime(),
        db: 'TEST',
        pos: '0.0',
        lane: 'KIỂM TRA THỦ CÔNG',
        tdoa: '0'
      };

      const caption = [
        isManualTest ? '🧪 KIỂM TRA BẰNG CHỨNG VI PHẠM' : '🚨 PHÁT HIỆN XE VI PHẠM TIẾNG ỒN',
        `• Thời gian: ${evInfo.dateTime}`,
        `• Độ ồn: ${evInfo.db} dB`,
        `• Phân làn: ${evInfo.lane}`,
        `• Vị trí: ${evInfo.pos} cm`,
        `• Độ lệch TDoA: ${evInfo.tdoa || '0'} μs`
      ].join('\n');

      try {
        let photoSent = false;
        let audioSent = false;

        // 1. Gửi ảnh bằng chứng (ưu tiên ảnh đã đóng dấu OSD, fallback tải /photo.jpg)
        let photoBlob = await createStampedPhotoBlob();
        if (!photoBlob) {
          const pRes = await fetch('/photo.jpg?tg=' + Date.now());
          if (pRes.ok) photoBlob = await pRes.blob();
        }
        if (photoBlob) {
          const photoForm = new FormData();
          photoForm.append('chat_id', chatId);
          photoForm.append('caption', caption);
          photoForm.append('photo', photoBlob, 'evidence.jpg');
          const resPhoto = await fetch(`https://api.telegram.org/bot${encodeURIComponent(token)}/sendPhoto`, {
            method: 'POST',
            body: photoForm
          });
          const photoJson = await resPhoto.json();
          if (!resPhoto.ok || !photoJson.ok) {
            throw new Error(photoJson.description || `HTTP ${resPhoto.status}`);
          }
          photoSent = true;
        }

        // 2. Gửi file âm thanh WAV 3s
        const aRes = await fetch('/audio.wav?tg=' + Date.now());
        if (aRes.ok) {
          const audioBlob = await aRes.blob();
          const audioForm = new FormData();
          audioForm.append('chat_id', chatId);
          audioForm.append('caption', `🔊 Bằng chứng âm thanh 3s (${evInfo.dateTime})`);
          audioForm.append('audio', audioBlob, 'evidence_3s.wav');
          const resAudio = await fetch(`https://api.telegram.org/bot${encodeURIComponent(token)}/sendAudio`, {
            method: 'POST',
            body: audioForm
          });
          const audioJson = await resAudio.json();
          if (!resAudio.ok || !audioJson.ok) {
            throw new Error(audioJson.description || `HTTP ${resAudio.status}`);
          }
          audioSent = true;
        }

        const timeDone = new Date().toLocaleTimeString('vi-VN');
        const detail = [photoSent ? 'Ảnh' : null, audioSent ? 'WAV' : null].filter(Boolean).join(' + ') || 'Tin nhắn';
        if (tgStatusText) {
          tgStatusText.textContent = `✅ Đã gửi (${detail}) lúc ${timeDone}`;
          tgStatusText.style.color = 'var(--green)';
        }
        if (!isManualTest) updateHistoryTgStatus(`✅ Đã gửi (${timeDone})`, 'var(--green)');
      } catch (err) {
        if (tgStatusText) {
          tgStatusText.textContent = `❌ Lỗi gửi Telegram: ${err.message}`;
          tgStatusText.style.color = 'var(--danger)';
        }
        if (!isManualTest) updateHistoryTgStatus('❌ Lỗi gửi', 'var(--danger)');
      }
    }

    function formatNowDateTime() {
      const now = new Date();
      const dateStr = now.getFullYear() + '-' +
                      String(now.getMonth() + 1).padStart(2, '0') + '-' +
                      String(now.getDate()).padStart(2, '0');
      const timeStr = String(now.getHours()).padStart(2, '0') + ':' +
                      String(now.getMinutes()).padStart(2, '0') + ':' +
                      String(now.getSeconds()).padStart(2, '0');
      return dateStr + ' ' + timeStr;
    }

    function triggerTestCapture() {
      activeViolationLane = 0;
      const photoTimeTag = document.getElementById('photoTimeTag');
      photoTimeTag.textContent = 'Đang chụp thử...';
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ cmd: "capture" }));
      } else {
        alert('Chưa kết nối WebSocket!');
      }
    }

    let ws;
    function connectWs() {
      const host = window.location.hostname || '192.168.4.1';
      ws = new WebSocket('ws://' + host + ':81/');

      ws.onopen = () => {
        connDot.className = 'dot';
        connText.textContent = 'Đã kết nối';
        syncSettingsToWs();
        ws.send(JSON.stringify({ cmd: "sync_time", time: Math.floor(Date.now() / 1000) }));

        const checkImg = new Image();
        checkImg.src = '/photo.jpg?check=' + Date.now();
        checkImg.onload = () => {
          const camPhoto = document.getElementById('camPhoto');
          const btnDlDirect = document.getElementById('btnDownloadPhotoDirect');
          const btnDl = document.getElementById('btnDownloadPhoto');
          if (camPhoto) {
            camPhoto.src = checkImg.src;
            camPhoto.onload = () => {
              renderPhotoToCanvas();
              if (btnDlDirect) btnDlDirect.style.display = 'inline-flex';
              if (btnDl) btnDl.style.display = 'inline-flex';
            };
          }
        };
      };

      ws.onmessage = (event) => {
        try {
          const d = JSON.parse(event.data);

          if (d.live) {
            const m1 = d.live.m1;
            const m2 = d.live.m2;
            m1DbEl.textContent = m1.toFixed(1) + ' dB';
            m2DbEl.textContent = m2.toFixed(1) + ' dB';
            m1Bar.style.width = Math.min(100, Math.max(0, (m1 / 120.0) * 100)) + '%';
            m2Bar.style.width = Math.min(100, Math.max(0, (m2 / 120.0) * 100)) + '%';
          }

          if (d.event) {
            const ev = d.event;
            pendingTgPhotoReady = false;
            pendingTgAudioReady = false;
            pendingTgSent = false;

            if (eventDbEl) eventDbEl.innerHTML = ev.db.toFixed(1) + ' <span style="font-size: 0.9rem; color: var(--dim);">dB</span>';

            targetCarPosPct = ev.pct;
            carOpacity = 1.0;
            pulseRadius = 1;
            lastEventTime = Date.now();

            const laneId = ev.lane || (ev.pct < LANE_1_MAX_PCT ? 1 : (ev.pct > LANE_2_MAX_PCT ? 3 : 2));
            activeViolationLane = laneId;

            let laneName = "LÀN 2 (GIỮA)";
            let laneClass = "var(--warn)";
            if (laneId === 1) {
              laneName = "LÀN 1 (TRÁI)";
              laneClass = "var(--green)";
            } else if (laneId === 3) {
              laneName = "LÀN 3 (PHẢI)";
              laneClass = "var(--accent)";
            }

            if (eventStatusEl) {
              eventStatusEl.textContent = "Đã bắt tiếng ồn vượt ngưỡng";
              eventStatusEl.style.color = "var(--danger)";
            }

            alertBanner.style.display = 'flex';
            alertMsg.textContent = `⚠️ PHÁT HIỆN TIẾNG ỒN ${ev.db.toFixed(1)} dB TẠI ${ev.pos.toFixed(1)} cm`;
            alertLane.textContent = laneName;
            alertLane.style.background = laneClass;
            alertLane.style.color = '#000';

            addHistoryRow(ev);

            const fullDateTime = formatNowDateTime();
            lastEventData = {
              dateTime: fullDateTime,
              db: ev.db.toFixed(1),
              pos: ev.pos.toFixed(1),
              lane: laneName,
              tdoa: (ev.tdoa > 0 ? '+' : '') + ev.tdoa.toFixed(0)
            };

            const audioSection = document.getElementById('audioSection');
            const audioStatus = document.getElementById('audioStatus');
            if (audioSection) audioSection.style.display = 'flex';
            if (audioStatus) {
              audioStatus.textContent = 'Đang thu 1.5s sau tiếng ồn...';
              audioStatus.style.color = 'var(--warn)';
            }

            const camPhoto = document.getElementById('camPhoto');
            const photoTimeTag = document.getElementById('photoTimeTag');
            const photoWatermark = document.getElementById('photoWatermark');
            const wmDateTime = document.getElementById('wmDateTime');
            const wmEventInfo = document.getElementById('wmEventInfo');
            const btnDownloadPhoto = document.getElementById('btnDownloadPhoto');
            const btnDownloadPhotoDirect = document.getElementById('btnDownloadPhotoDirect');

            wmDateTime.textContent = fullDateTime;
            wmEventInfo.textContent = `${ev.db.toFixed(1)} dB | ${laneName} (${ev.pos.toFixed(1)} cm)`;

            camPhoto.src = '/photo.jpg?t=' + Date.now();
            camPhoto.onload = () => {
              renderPhotoToCanvas();
              photoWatermark.style.display = 'block';
              btnDownloadPhoto.style.display = 'inline-flex';
              if (btnDownloadPhotoDirect) btnDownloadPhotoDirect.style.display = 'inline-flex';
              photoTimeTag.textContent = fullDateTime;
              pendingTgPhotoReady = true;
              tryAutoSendTelegram();
            };
          }

          if (d.audio_ready) {
            const audioPlayer = document.getElementById('audioPlayer');
            const btnDownloadAudio = document.getElementById('btnDownloadAudio');
            const audioStatus = document.getElementById('audioStatus');
            const audioSection = document.getElementById('audioSection');
            if (audioSection) audioSection.style.display = 'flex';
            if (audioPlayer) {
              audioPlayer.src = '/audio.wav?t=' + Date.now();
            }
            if (btnDownloadAudio) btnDownloadAudio.style.display = 'inline-flex';
            if (audioStatus) {
              audioStatus.textContent = 'Sẵn sàng (3s WAV)';
              audioStatus.style.color = 'var(--green)';
            }
            pendingTgAudioReady = true;
            tryAutoSendTelegram();
          }

          if (d.cam_ok) {
            const camPhoto = document.getElementById('camPhoto');
            const photoTimeTag = document.getElementById('photoTimeTag');
            const btnDownloadPhoto = document.getElementById('btnDownloadPhoto');
            const btnDownloadPhotoDirect = document.getElementById('btnDownloadPhotoDirect');
            const fullDateTime = formatNowDateTime();
            lastEventData = { dateTime: fullDateTime, db: 'TEST', pos: '0.0', lane: 'CHỤP THỬ', tdoa: '0' };
            activeViolationLane = 0;
            camPhoto.src = '/photo.jpg?t=' + Date.now();
            camPhoto.onload = () => {
              renderPhotoToCanvas();
              btnDownloadPhoto.style.display = 'inline-flex';
              if (btnDownloadPhotoDirect) btnDownloadPhotoDirect.style.display = 'inline-flex';
              photoTimeTag.textContent = 'Chụp thử: ' + fullDateTime;
            };
          }

          if (d.cam_err) {
            const photoTimeTag = document.getElementById('photoTimeTag');
            photoTimeTag.textContent = 'Lỗi camera: ' + d.code;
            alertBanner.style.display = 'flex';
            alertMsg.textContent = '⚠️ LỖI CAMERA (' + d.code + ')';
          }
        } catch (e) {
          console.error(e);
        }
      };

      ws.onclose = () => {
        connDot.className = 'dot offline';
        connText.textContent = 'Mất kết nối...';
        setTimeout(connectWs, 2000);
      };

      ws.onerror = () => {
        connDot.className = 'dot offline';
        connText.textContent = 'Lỗi kết nối';
      };
    }

    loadTelegramConfig();
    updateRotBtnLabel();
    updateROIBtnLabel();
    connectWs();
    syncSettingsToWs();
  </script>
</body>
</html>
)rawliteral";
