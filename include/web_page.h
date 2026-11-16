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
      --bg: #070b14;
      --card: #0f172a;
      --card-hover: #17223b;
      --border: #1e293b;
      --accent: #00f0ff;
      --danger: #ff0055;
      --warn: #ffb703;
      --green: #00ff88;
      --text: #f8fafc;
      --dim: #94a3b8;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      background: var(--bg);
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", sans-serif;
      padding: 16px;
      display: flex;
      flex-direction: column;
      align-items: center;
      min-height: 100vh;
    }
    .container {
      width: 100%;
      max-width: 980px;
      display: flex;
      flex-direction: column;
      gap: 16px;
    }
    .header {
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 14px;
      padding: 14px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
      gap: 12px;
      box-shadow: 0 4px 20px rgba(0,0,0,0.3);
    }
    .title h1 {
      font-size: 1.25rem;
      font-weight: 800;
      letter-spacing: 0.5px;
    }
    .title h1 span { color: var(--accent); }
    .title p {
      font-size: 0.78rem;
      color: var(--dim);
      margin-top: 2px;
    }
    .badges {
      display: flex;
      gap: 8px;
      align-items: center;
    }
    .badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      font-size: 0.8rem;
      padding: 6px 12px;
      border-radius: 20px;
      background: rgba(0,240,255,0.08);
      color: var(--accent);
      border: 1px solid rgba(0,240,255,0.25);
    }
    .dot {
      width: 8px; height: 8px; border-radius: 50%;
      background: var(--green); box-shadow: 0 0 8px var(--green);
      transition: background 0.3s, box-shadow 0.3s;
    }
    .dot.offline {
      background: var(--danger);
      box-shadow: 0 0 8px var(--danger);
    }

    /* Banner Cảnh Báo */
    .alert-banner {
      display: none;
      background: rgba(255, 0, 85, 0.12);
      border: 1px solid var(--danger);
      border-radius: 12px;
      padding: 12px 18px;
      color: #ff3377;
      font-weight: 600;
      align-items: center;
      justify-content: space-between;
      animation: pulseAlert 0.8s infinite alternate ease-in-out;
    }
    @keyframes pulseAlert {
      0% { opacity: 0.85; transform: scale(1.0); }
      100% { opacity: 1; transform: scale(1.006); }
    }

    /* Khung Mô Phỏng Con Đường 2D */
    .road-section {
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 14px;
      padding: 18px 20px 22px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }
    .road-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 0.85rem;
      color: var(--dim);
      font-weight: 600;
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }
    .road-canvas-wrap {
      width: 100%;
      height: 240px;
      position: relative;
    }
    canvas {
      width: 100%;
      height: 100%;
      border-radius: 10px;
      background: #090e1a;
      display: block;
    }

    /* Các Thẻ Thông Số */
    .cards {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
      gap: 12px;
    }
    .card {
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 16px;
      display: flex;
      flex-direction: column;
      gap: 6px;
      transition: background 0.2s, border-color 0.2s;
    }
    .card:hover {
      background: var(--card-hover);
      border-color: rgba(0,240,255,0.2);
    }
    .card-title {
      font-size: 0.75rem;
      color: var(--dim);
      text-transform: uppercase;
      letter-spacing: 0.5px;
      font-weight: 600;
    }
    .card-val {
      font-size: 2rem;
      font-weight: 800;
      line-height: 1.1;
    }
    .card-sub {
      font-size: 0.8rem;
      color: var(--dim);
    }

    /* VU Meter Thanh Độ Ồn Realtime */
    .vu-bar-wrap {
      display: flex;
      flex-direction: column;
      gap: 6px;
      margin-top: 6px;
    }
    .vu-row {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 0.78rem;
    }
    .vu-label {
      width: 26px;
      font-weight: 700;
    }
    .vu-track {
      flex: 1;
      height: 8px;
      background: #1e293b;
      border-radius: 4px;
      overflow: hidden;
      position: relative;
    }
    .vu-fill {
      height: 100%;
      width: 0%;
      background: linear-gradient(90deg, var(--green) 0%, var(--warn) 60%, var(--danger) 90%);
      transition: width 0.08s ease-out;
      border-radius: 4px;
    }
    .vu-thresh-line {
      position: absolute;
      top: 0;
      bottom: 0;
      width: 2px;
      background: #ffffff;
      box-shadow: 0 0 4px #ffffff;
    }
    .vu-val {
      width: 52px;
      text-align: right;
      font-family: monospace;
      font-weight: 700;
    }

    /* Bảng Cài Đặt */
    .settings-panel {
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 14px;
      padding: 16px 20px;
      display: flex;
      flex-wrap: wrap;
      gap: 20px;
      align-items: center;
      justify-content: space-between;
    }
    .slider-group {
      display: flex;
      flex-direction: column;
      gap: 8px;
      min-width: 260px;
      flex: 1;
    }
    .slider-header {
      display: flex;
      justify-content: space-between;
      font-size: 0.85rem;
      font-weight: 600;
    }
    .slider-header span { color: var(--dim); }
    .slider-header b {
      color: var(--accent);
      font-family: monospace;
      font-size: 1rem;
    }
    .slider-ctrl {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .btn-step {
      background: #1e293b;
      color: var(--text);
      border: 1px solid #334155;
      border-radius: 6px;
      width: 32px;
      height: 32px;
      font-size: 1.1rem;
      font-weight: 700;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      transition: background 0.15s, border-color 0.15s;
    }
    .btn-step:hover {
      background: #334155;
      border-color: var(--accent);
      color: var(--accent);
    }
    input[type=range] {
      flex: 1;
      height: 6px;
      border-radius: 3px;
      accent-color: var(--accent);
      cursor: pointer;
      background: #1e293b;
    }

    /* Bảng Lịch Sử Sự Kiện */
    .history-card {
      background: var(--card);
      border: 1px solid var(--border);
      border-radius: 14px;
      padding: 16px 20px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }
    .history-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 0.85rem;
      font-weight: 700;
      color: var(--dim);
      text-transform: uppercase;
      letter-spacing: 0.5px;
    }
    .btn-clear {
      background: transparent;
      border: 1px solid #334155;
      color: var(--dim);
      padding: 4px 10px;
      border-radius: 6px;
      font-size: 0.75rem;
      cursor: pointer;
      transition: all 0.15s;
    }
    .btn-clear:hover {
      background: rgba(255, 0, 85, 0.15);
      border-color: var(--danger);
      color: var(--danger);
    }
    .table-wrap {
      overflow-x: auto;
    }
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
      font-size: 0.78rem;
      color: var(--dim);
      padding: 8px 0 16px;
    }
  </style>
</head>
<body>
  <div class="container">
    <!-- Header -->
    <div class="header">
      <div class="title">
        <h1><span>ESP32-S3</span> ĐỊNH VỊ TIẾNG ỒN TRÊN LÀN ĐƯỜNG</h1>
        <p>Thuật toán TDoA Cross-Correlation • Bộ lọc trung bình 30 mẫu</p>
      </div>
      <div class="badges">
        <div class="badge">
          <div id="connDot" class="dot"></div>
          <span id="connText">Đã kết nối</span>
        </div>
        <div class="badge" style="border-color: rgba(255,183,3,0.3); color: var(--warn); background: rgba(255,183,3,0.08);">
          <span>TB 30 lần</span>
        </div>
      </div>
    </div>

    <!-- Banner Cảnh Báo -->
    <div id="alertBanner" class="alert-banner">
      <span id="alertMsg">⚠️ PHÁT HIỆN TIẾNG ỒN VƯỢT NGƯỠNG!</span>
      <span id="alertLane" class="lane-pill" style="background: var(--danger); color: #fff;">LÀN TRÁI</span>
    </div>

    <!-- Mô Phỏng Con Đường 2D -->
    <div class="road-section">
      <div class="road-header">
        <span>MÔ PHỎNG LÒNG ĐƯỜNG VÀ VỊ TRÍ NGUỒN TIẾNG ỒN</span>
        <span id="roadDimLabel">Bề rộng đường: 30 cm</span>
      </div>
      <div class="road-canvas-wrap">
        <canvas id="roadCanvas"></canvas>
      </div>
    </div>

    <!-- Các Thẻ Thông Số -->
    <div class="cards">
      <div class="card">
        <div class="card-title">Mức Tiếng Ồn Gây Ra (TB 30 Lần)</div>
        <div class="card-val" id="eventDb" style="color: var(--danger);">-- <span style="font-size: 1rem; color: var(--dim);">dB</span></div>
        <div class="card-sub" id="eventStatus">Chờ âm thanh vượt ngưỡng...</div>
      </div>

      <div class="card">
        <div class="card-title">Vị Trí Trên Lòng Đường</div>
        <div class="card-val" id="eventPos" style="color: var(--accent);">-- <span style="font-size: 1rem; color: var(--dim);">cm</span></div>
        <div class="card-sub" id="eventLaneDesc">Tính từ mép Mic 1 (Lề trái)</div>
      </div>

      <div class="card">
        <div class="card-title">Độ Lệch Thời Gian (TDoA)</div>
        <div class="card-val" id="eventTdoa" style="color: var(--warn);">-- <span style="font-size: 1rem; color: var(--dim);">μs</span></div>
        <div class="card-sub" id="tdoaDesc">Δt = t₁ - t₂ (Trung bình 30 mẫu)</div>
      </div>

      <div class="card">
        <div class="card-title">Độ Ồn Trực Tiếp 2 Micro</div>
        <div class="vu-bar-wrap">
          <div class="vu-row">
            <span class="vu-label" style="color: var(--green);">M1</span>
            <div class="vu-track">
              <div id="m1Bar" class="vu-fill"></div>
              <div id="m1Thresh" class="vu-thresh-line" style="left: 30%;"></div>
            </div>
            <span id="m1Db" class="vu-val">-- dB</span>
          </div>
          <div class="vu-row">
            <span class="vu-label" style="color: var(--accent);">M2</span>
            <div class="vu-track">
              <div id="m2Bar" class="vu-fill"></div>
              <div id="m2Thresh" class="vu-thresh-line" style="left: 30%;"></div>
            </div>
            <span id="m2Db" class="vu-val">-- dB</span>
          </div>
        </div>
        <div class="card-sub" style="margin-top: 4px;">Cập nhật 10 lần/s • Vạch trắng: Ngưỡng</div>
      </div>
    </div>

    <!-- Khung Bằng Chứng: Ảnh Chụp & Đoạn Ghi Âm 3 Giây -->
    <div class="card">
      <div style="display: flex; justify-content: space-between; align-items: center; flex-wrap: wrap; gap: 8px;">
        <div class="card-title">Bằng Chứng Vi Phạm (Ảnh Chụp + Ghi Âm 3 Giây)</div>
        <div style="display: flex; gap: 8px; align-items: center; flex-wrap: wrap;">
          <div id="photoTimeTag" style="font-size: 0.75rem; color: var(--dim); font-family: monospace;">Chưa có bằng chứng</div>
          <button class="btn-clear" onclick="triggerTestCapture()" style="color:var(--accent); border-color:rgba(0,240,255,0.4); font-weight: 600;">📸 Chụp thử ngay</button>
          <button id="btnToggleROI" class="btn-clear" onclick="toggleLaneROI()" style="color:var(--green); border-color:rgba(0,255,136,0.4); font-weight: 600;">📐 3 Làn: BẬT</button>
          <button class="btn-clear" onclick="cycleRotation()" title="Nhấn để đổi góc xoay ảnh (270° / 0° / 90° / 180°)" style="color:var(--accent); border-color:rgba(0,240,255,0.4); font-weight: 600;">🔄 Xoay: <span id="rotDeg">270°</span></button>
          <button id="btnDownloadPhotoDirect" class="btn-clear" onclick="downloadRotatedPhoto()" style="display:none; color: var(--accent); border-color: rgba(0,240,255,0.4); font-weight: 600;">⬇ Tải ảnh JPG</button>
          <button id="btnDownloadPhoto" class="btn-clear" onclick="downloadStampedPhoto()" style="display:none; color: var(--warn); border-color: rgba(255,183,3,0.4);">⬇ Tải ảnh OSD</button>
          <a id="btnDownloadAudio" href="/audio.wav" download="evidence_3s.wav" class="btn-clear" style="display:none; color: var(--green); border-color: rgba(0,255,136,0.4); text-decoration: none;">⬇ Tải file WAV (3s)</a>
        </div>
      </div>
      <!-- Khung hiển thị song song 2 ảnh: Toàn cảnh & Zoom cận cảnh biển số -->
      <div style="display: flex; gap: 12px; margin-top: 8px; flex-wrap: wrap; align-items: stretch;">
        <!-- Khung 1: Ảnh Toàn Cảnh (Lưới 3 Làn & Khoanh Đỏ Xe Vi Phạm) -->
        <div style="flex: 2; min-width: 290px; min-height: 260px; max-height: 480px; background: #070b14; border-radius: 8px; border: 1px dashed #1e293b; display: flex; flex-direction: column; align-items: center; justify-content: center; overflow: hidden; position: relative;">
          <div style="position: absolute; top: 8px; left: 10px; z-index: 5; background: rgba(7,11,20,0.85); border: 1px solid var(--accent); padding: 3px 8px; border-radius: 4px; font-size: 0.72rem; color: var(--accent); font-weight: 700;">
            📷 ẢNH TOÀN CẢNH (KHOANH VÙNG XE)
          </div>
          <img id="camPhoto" src="" alt="Chưa có ảnh chụp" style="display: none;">
          <canvas id="camCanvas" style="display: none; max-width: 100%; max-height: 460px; border-radius: 6px; object-fit: contain; box-shadow: 0 4px 16px rgba(0,0,0,0.5);"></canvas>
          
          <!-- Nhãn đóng dấu ngày giờ OSD trực tiếp trên ảnh -->
          <div id="photoWatermark" style="display: none; position: absolute; bottom: 10px; left: 10px; background: rgba(7, 11, 20, 0.85); border: 1px solid rgba(0,240,255,0.4); border-left: 4px solid var(--accent); padding: 4px 10px; border-radius: 6px; font-family: monospace; line-height: 1.3; text-shadow: 1px 1px 2px #000; pointer-events: none; z-index: 5;">
            <div id="wmDateTime" style="font-weight: 700; color: #00f0ff; font-size: 0.8rem;">2026-11-13 17:04:36</div>
            <div id="wmEventInfo" style="font-size: 0.75rem; color: #ffb703;">85.0 dB | LÀN TRÁI (12.3 cm)</div>
          </div>

          <span id="camPhotoPlaceholder" style="color: var(--dim); font-size: 0.85rem;">Tự động chụp và ghi âm 3s khi tiếng ồn vượt ngưỡng</span>
        </div>

        <!-- Khung 2: Ảnh Cận Cảnh Đuôi Xe / Biển Số (Zoom Crop) -->
        <div id="plateBoxWrap" style="flex: 1; min-width: 230px; min-height: 260px; max-height: 480px; background: #070b14; border-radius: 8px; border: 1px solid #1e293b; display: none; flex-direction: column; align-items: center; justify-content: space-between; padding: 10px; position: relative;">
          <div style="width: 100%; display: flex; justify-content: space-between; align-items: center; margin-bottom: 6px;">
            <span style="font-size: 0.75rem; font-weight: 700; color: #ff0055; text-transform: uppercase;">🔍 CẬN CẢNH ĐUÔI XE / BIỂN SỐ</span>
            <span id="cropLaneBadge" style="font-size: 0.7rem; background: rgba(255,0,85,0.2); color: #ff0055; border: 1px solid rgba(255,0,85,0.4); padding: 2px 6px; border-radius: 4px; font-weight: 700;">LÀN 2</span>
          </div>

          <div style="width: 100%; flex: 1; min-height: 180px; display: flex; align-items: center; justify-content: center; position: relative; overflow: hidden; border-radius: 6px; border: 2px solid #ff0055; background: #000;">
            <canvas id="plateCropCanvas" style="width: 100%; height: 100%; object-fit: contain;"></canvas>
            <!-- Crosshair ngắm biển số -->
            <div style="position: absolute; inset: 10px; border: 1px dashed rgba(255, 0, 85, 0.7); pointer-events: none; border-radius: 4px;"></div>
            <div style="position: absolute; bottom: 6px; right: 8px; font-size: 0.65rem; color: #ff0055; font-family: monospace; font-weight: 700; background: rgba(0,0,0,0.7); padding: 2px 4px; border-radius: 2px;">ZOOM 2.5X</div>
          </div>

          <div style="width: 100%; display: flex; justify-content: space-between; align-items: center; margin-top: 8px;">
            <span style="font-size: 0.7rem; color: var(--dim);">Cắt tự động làn vi phạm</span>
            <button class="btn-clear" onclick="downloadPlateCrop()" style="font-size: 0.72rem; padding: 4px 8px; color: var(--danger); border-color: rgba(255,0,85,0.4); font-weight: 600;">⬇ Tải ảnh biển số</button>
          </div>
        </div>
      </div>

      <!-- Trình phát đoạn ghi âm 3 giây -->
      <div id="audioSection" style="margin-top: 10px; display: none; flex-direction: column; gap: 6px;">
        <div style="display: flex; justify-content: space-between; align-items: center; font-size: 0.8rem;">
          <span style="color: var(--dim);">🔊 Đoạn ghi âm 3 giây (1.5s trước + 1.5s sau tiếng ồn):</span>
          <span id="audioStatus" style="color: var(--warn); font-weight: 600;">Đang thu 1.5s sau...</span>
        </div>
        <audio id="audioPlayer" controls style="width: 100%; height: 38px; border-radius: 6px;"></audio>
      </div>
    </div>

    <!-- Bảng Cài Đặt -->
    <div class="settings-panel">
      <div class="slider-group">
        <div class="slider-header">
          <span>Ngưỡng Bắt Tiếng Ồn:</span>
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
          <span>Bề Rộng Mô Hình Đường:</span>
          <b id="widthVal">30 cm</b>
        </div>
        <div class="slider-ctrl">
          <button class="btn-step" onclick="stepWidth(-5)">-</button>
          <input type="range" id="widthSlider" min="10" max="100" value="30" autocomplete="off" oninput="onWidthInput()">
          <button class="btn-step" onclick="stepWidth(5)">+</button>
        </div>
      </div>
    </div>

    <!-- Lịch Sử Bắt Tiếng Ồn -->
    <div class="history-card">
      <div class="history-header">
        <span>Lịch Sử Phát Hiện Tiếng Ồn Gần Nhất</span>
        <button class="btn-clear" onclick="clearHistory()">Xóa lịch sử</button>
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
            </tr>
          </thead>
          <tbody id="historyBody">
            <tr id="emptyRow">
              <td colspan="6" style="text-align: center; color: var(--dim); padding: 16px;">Chưa có sự kiện nào được ghi nhận.</td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>

    <div class="footer">
      ESP32-S3 CAM • 2x INMP441 Microphones (I2S Stereo 48kHz) • TDoA Cross-Correlation
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
    const eventPosEl = document.getElementById('eventPos');
    const eventTdoaEl = document.getElementById('eventTdoa');
    const eventStatusEl = document.getElementById('eventStatus');
    const eventLaneDesc = document.getElementById('eventLaneDesc');

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
    const emptyRow = document.getElementById('emptyRow');

    let roadWidth = parseFloat(localStorage.getItem('cfg_width') || '30');
    let threshold = parseFloat(localStorage.getItem('cfg_thresh') || '75');

    // Đồng bộ vị trí thanh trượt với giá trị số ngay khi tải trang
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
    const historyList = [];

    function resizeCanvas() {
      const dpr = window.devicePixelRatio || 1;
      const rect = canvas.getBoundingClientRect();
      canvas.width = rect.width * dpr;
      canvas.height = rect.height * dpr;
      ctx.resetTransform ? ctx.resetTransform() : ctx.setTransform(1, 0, 0, 1, 0, 0);
      ctx.scale(dpr, dpr);
    }
    window.addEventListener('resize', resizeCanvas);
    resizeCanvas();

    function drawRoad() {
      const w = canvas.getBoundingClientRect().width;
      const h = canvas.getBoundingClientRect().height;
      ctx.clearRect(0, 0, w, h);

      const marginX = 70;
      const roadTop = 32;
      const roadH = h - 64;
      const roadW = w - marginX * 2;
      const centerY = roadTop + roadH / 2;

      // 1. Mặt đường chính
      ctx.fillStyle = '#111827';
      ctx.fillRect(marginX, roadTop, roadW, roadH);

      // 2. Viền mép đường (Lề trắng)
      ctx.fillStyle = '#f8fafc';
      ctx.fillRect(marginX, roadTop, roadW, 4);
      ctx.fillRect(marginX, roadTop + roadH - 4, roadW, 4);

      // 3. Hai vạch phân làn (nét đứt) chia lòng đường thành 3 làn
      const xLane1 = marginX + roadW * 0.35;
      const xLane2 = marginX + roadW * 0.68;

      ctx.strokeStyle = '#ffb703';
      ctx.lineWidth = 2.5;
      ctx.setLineDash([12, 10]);

      // Vạch 1 (giữa Làn 1 & Làn 2)
      ctx.beginPath();
      ctx.moveTo(xLane1, roadTop);
      ctx.lineTo(xLane1, roadTop + roadH);
      ctx.stroke();

      // Vạch 2 (giữa Làn 2 & Làn 3)
      ctx.beginPath();
      ctx.moveTo(xLane2, roadTop);
      ctx.lineTo(xLane2, roadTop + roadH);
      ctx.stroke();
      ctx.setLineDash([]);

      // Nhãn 3 làn đường
      ctx.fillStyle = 'rgba(148, 163, 184, 0.4)';
      ctx.font = 'bold 11px -apple-system, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('LÀN 1 (TRÁI)', marginX + roadW * 0.175, roadTop + roadH * 0.5);
      ctx.fillText('LÀN 2 (GIỮA)', marginX + roadW * 0.515, roadTop + roadH * 0.5);
      ctx.fillText('LÀN 3 (PHẢI)', marginX + roadW * 0.84, roadTop + roadH * 0.5);

      // Thước đo cm (Top ruler)
      ctx.fillStyle = '#64748b';
      ctx.font = '9px monospace';
      const steps = 4;
      for (let s = 0; s <= steps; s++) {
        const xTick = marginX + (s / steps) * roadW;
        const cmVal = ((s / steps) * roadWidth).toFixed(0);
        ctx.fillRect(xTick - 0.5, roadTop - 7, 1, 5);
        ctx.fillText(`${cmVal}cm`, xTick, roadTop - 11);
      }

      // 4. Hai trạm Micro hai bên
      // MIC 1 (Trái)
      ctx.fillStyle = '#00ff88';
      ctx.shadowColor = '#00ff88';
      ctx.shadowBlur = 10;
      ctx.beginPath();
      ctx.arc(marginX, centerY, 10, 0, Math.PI * 2);
      ctx.fill();
      ctx.shadowBlur = 0;

      ctx.fillStyle = '#00ff88';
      ctx.font = 'bold 11px -apple-system, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('MIC 1', marginX - 36, centerY - 4);
      ctx.font = '10px -apple-system, sans-serif';
      ctx.fillStyle = '#94a3b8';
      ctx.fillText('(0 cm)', marginX - 36, centerY + 11);

      // MIC 2 (Phải)
      ctx.fillStyle = '#00f0ff';
      ctx.shadowColor = '#00f0ff';
      ctx.shadowBlur = 10;
      ctx.beginPath();
      ctx.arc(marginX + roadW, centerY, 10, 0, Math.PI * 2);
      ctx.fill();
      ctx.shadowBlur = 0;

      ctx.fillStyle = '#00f0ff';
      ctx.font = 'bold 11px -apple-system, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('MIC 2', marginX + roadW + 36, centerY - 4);
      ctx.font = '10px -apple-system, sans-serif';
      ctx.fillStyle = '#94a3b8';
      ctx.fillText(`(${roadWidth} cm)`, marginX + roadW + 36, centerY + 11);

      // 5. Nội suy vị trí xe
      currentCarPosPct += (targetCarPosPct - currentCarPosPct) * 0.16;
      const carX = marginX + (currentCarPosPct / 100.0) * roadW;

      // 6. Sóng lan tỏa khi phát hiện tiếng ồn
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

      // 7. Điểm xe / Nguồn âm thanh
      ctx.save();
      ctx.translate(carX, centerY);
      ctx.fillStyle = `rgba(255, 0, 85, ${carOpacity})`;
      ctx.shadowColor = '#ff0055';
      ctx.shadowBlur = carOpacity > 0.6 ? 24 : 6;

      // Thân xe
      ctx.beginPath();
      ctx.roundRect ? ctx.roundRect(-20, -13, 40, 26, 4) : ctx.rect(-20, -13, 40, 26);
      ctx.fill();

      // Kính xe
      ctx.fillStyle = '#ffffff';
      ctx.fillRect(-7, -10, 14, 20);

      // Đèn vàng
      ctx.fillStyle = '#ffb703';
      ctx.fillRect(17, -11, 4, 6);
      ctx.fillRect(17, 5, 4, 6);
      ctx.restore();

      // Tag khoảng cách phía trên
      ctx.fillStyle = carOpacity > 0.6 ? '#ffffff' : '#94a3b8';
      ctx.font = 'bold 11px monospace';
      ctx.textAlign = 'center';
      const distFromLeft = ((currentCarPosPct / 100.0) * roadWidth).toFixed(1);
      ctx.fillText(`${distFromLeft} cm (${currentCarPosPct.toFixed(0)}%)`, carX, roadTop - 18);

      // Đường gióng đứt nét
      ctx.strokeStyle = `rgba(255, 0, 85, ${carOpacity > 0.6 ? 0.6 : 0.2})`;
      ctx.lineWidth = 1.5;
      ctx.setLineDash([4, 4]);
      ctx.beginPath();
      ctx.moveTo(carX, roadTop);
      ctx.lineTo(carX, roadTop + roadH);
      ctx.stroke();
      ctx.setLineDash([]);

      // Tự mờ dần sau 5 giây không có sự kiện mới
      if (Date.now() - lastEventTime > 5000) {
        carOpacity = Math.max(0.25, carOpacity - 0.015);
        alertBanner.style.display = 'none';
      }

      requestAnimationFrame(drawRoad);
    }
    requestAnimationFrame(drawRoad);

    // Cập nhật giao diện cài đặt & gửi WebSocket
    let sendTimeout = null;
    function syncSettingsToWs() {
      if (threshSlider) threshSlider.value = threshold;
      if (widthSlider) widthSlider.value = roadWidth;
      if (threshVal) threshVal.textContent = threshold + ' dB';
      if (widthVal) widthVal.textContent = roadWidth + ' cm';
      if (roadDimLabel) roadDimLabel.textContent = `Bề rộng đường: ${roadWidth} cm`;

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
      if (threshSlider) threshSlider.value = threshold;
      localStorage.setItem('cfg_thresh', threshold);
      syncSettingsToWs();
    }

    function stepWidth(delta) {
      roadWidth = Math.min(100, Math.max(10, roadWidth + delta));
      if (widthSlider) widthSlider.value = roadWidth;
      localStorage.setItem('cfg_width', roadWidth);
      syncSettingsToWs();
    }

    function addHistoryRow(ev) {
      if (emptyRow) emptyRow.remove();

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
      `;

      historyBody.insertBefore(tr, historyBody.firstChild);

      // Giữ tối đa 10 dòng
      while (historyBody.children.length > 10) {
        historyBody.removeChild(historyBody.lastChild);
      }
    }

    function clearHistory() {
      historyBody.innerHTML = `
        <tr id="emptyRow">
          <td colspan="6" style="text-align: center; color: var(--dim); padding: 16px;">Chưa có sự kiện nào được ghi nhận.</td>
        </tr>
      `;
    }

    const LANE_1_MAX_PCT = 38.5;
    const LANE_2_MAX_PCT = 74.0;
    let showLaneROI = (localStorage.getItem('show_lane_roi') !== 'false'); // Mặc định BẬT hiển thị khoanh vùng xe vi phạm
    let activeViolationLane = 0; // 0: Chưa có, 1: Làn 1, 2: Làn 2, 3: Làn 3
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

      // Vùng quét chính xác nằm gọn trong mặt đường trắng, né hoàn toàn 2 vạch băng dính đen (165-205 và 335-375)
      let scanX1, scanX2;
      if (targetLane === 1) {
        scanX1 = 28;
        scanX2 = 160;
      } else if (targetLane === 2) {
        scanX1 = 212;
        scanX2 = 328;
      } else {
        scanX1 = 380;
        scanX2 = 465;
      }

      // Giới hạn Y trên toàn bộ mặt đường trắng (từ đỉnh đường Y=255 đến Y=615)
      const scanY1 = 255;
      const scanY2 = 615;
      const sw = scanX2 - scanX1;
      const sh = scanY2 - scanY1;
      if (sw <= 15 || sh <= 20) return null;

      try {
        const imgData = ctx.getImageData(scanX1, scanY1, sw, sh);
        const data = imgData.data;

        // 1. Phân loại pixel xe thô (Raw Mask)
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

            // Loại trừ mặt đường trắng và bóng mờ trung tính
            const isWhiteRoad = (minC > 180 && sat < 0.10);
            const isNeutralShadow = (minC > 115 && diffC < 20 && sat < 0.11);

            // Nhận diện thân xe mô hình (màu sắc rõ rệt hoặc sẫm màu)
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

        // 2. Co điểm ảnh 2 lần (Erosion x2): Lọc sạch tia nhọn, đốm gai nhỏ
        let curMask = rawMask;
        for (let pass = 0; pass < 2; pass++) {
          const nextMask = new Uint8Array(sw * sh);
          for (let y = 1; y < sh - 1; y++) {
            for (let x = 1; x < sw - 1; x++) {
              const idx = y * sw + x;
              if (curMask[idx] &&
                  curMask[idx - 1] && curMask[idx + 1] &&
                  curMask[idx - sw] && curMask[idx + sw]) {
                nextMask[idx] = 1;
              }
            }
          }
          curMask = nextMask;
        }

        // 3. Dãn điểm ảnh (Dilation 5x5): Hàn gắn viền bao quanh thân xe
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

        // 4. Lấp đặc ruột (Hole Filling via Flood-Fill BFS từ 4 cạnh ngoài):
        // Mọi pixel 0 không thông được ra viền ngoài đều là lỗ thủng kín -> biến thành 1
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
          const neighbors = [
            [cx - 1, cy], [cx + 1, cy],
            [cx, cy - 1], [cx, cy + 1]
          ];
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

        // Tạo cleanMask đặc ruột (Solid):
        const cleanMask = new Uint8Array(sw * sh);
        for (let i = 0; i < sw * sh; i++) {
          cleanMask[i] = exterior[i] ? 0 : 1;
        }

        // 4. Quét mật độ theo từng hàng ngang từ cleanMask sau xử lý hình thái học
        const rowSums = new Array(sh).fill(0);
        for (let y = 0; y < sh; y += 2) {
          for (let x = 0; x < sw; x += 2) {
            if (cleanMask[y * sw + x]) {
              rowSums[y]++;
            }
          }
        }

        // Quét tìm các cụm hàng liên tục (cluster), chặn nhảy cóc xuống đáy đường
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
            // Gặp khoảng trống trắng > 12px -> ngắt cụm xe
            if (gap > 12) {
              const clusterHeight = curEnd - curStart;
              if (curDense >= 10 && clusterHeight >= 30) {
                if (!bestCluster || curDense > bestCluster.dense) {
                  bestCluster = { start: curStart, end: curEnd, dense: curDense };
                }
              }
              curStart = -1;
              curEnd = -1;
              curDense = 0;
              gap = 0;
            }
          }
        }
        if (curStart !== -1 && curDense >= 10 && (curEnd - curStart >= 30)) {
          if (!bestCluster || curDense > bestCluster.dense) {
            bestCluster = { start: curStart, end: curEnd, dense: curDense };
          }
        }

        // Chỉ chấp nhận khi tìm thấy cụm xe rõ ràng
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

          // Lọc kích thước & tỉ lệ khung hình (xe không thể cao gấp 3.2 lần rộng)
          if (carW >= 20 && carH >= 25 && (carH / carW <= 3.0)) {
            const padX = 6;
            const padY = 6;
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

      // Làn không có xe -> trả về null (KHÔNG khoanh)
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

      // Quét phát hiện xe trên cả 3 làn (chỉ làn nào có xe mới trả về box)
      const detectedCars = [];
      for (let l = 1; l <= 3; l++) {
        const vbox = findVehicleBoundingBox(ctx, w, h, l);
        if (vbox) {
          detectedCars.push(vbox);
        }
      }

      // Chọn xe để zoom cận cảnh biển số
      let violatorCar = null;
      let focusCar = null;

      if (hasSoundViolation) {
        violatorCar = detectedCars.find(c => c.lane === activeLane) || null;
        focusCar = violatorCar || detectedCars[0] || null;
      } else {
        focusCar = detectedCars[0] || null;
      }
      lastDetectedVBox = focusCar;

      // 1. Highlight nền làn vi phạm (Chỉ khi có âm thanh vượt ngưỡng thực sự)
      if (hasSoundViolation) {
        let rx = (activeLane === 1) ? 0 : (activeLane === 2 ? x1 : x2);
        let rw = (activeLane === 1) ? x1 : (activeLane === 2 ? x2 - x1 : w - x2);

        ctx.fillStyle = 'rgba(255, 0, 85, 0.18)';
        ctx.fillRect(rx, topY, rw, bottomY - topY);

        ctx.strokeStyle = '#ff0055';
        ctx.lineWidth = 2;
        ctx.strokeRect(rx + 1, topY + 1, rw - 2, bottomY - topY - 2);
      }

      // 2. Vẽ Bounding Box:
      //    - Xe ở làn vi phạm âm thanh: KHOANH ĐỎ
      //    - Xe không vi phạm (mặc định / khi test chưa có tiếng): KHOANH MÀU XANH
      //    - Làn không có xe: KHÔNG KHOANH GÌ
      detectedCars.forEach(car => {
        const isViolator = (hasSoundViolation && car.lane === activeLane);

        if (isViolator) {
          // XE VI PHẠM -> KHOANH ĐỎ
          ctx.strokeStyle = '#ff0055';
          ctx.lineWidth = 3.5;
          ctx.strokeRect(car.x, car.y, car.w, car.h);

          // 4 góc bracket cyan
          const cLen = Math.min(16, car.w / 4);
          ctx.strokeStyle = '#00f0ff';
          ctx.lineWidth = 2.5;
          ctx.beginPath(); ctx.moveTo(car.x, car.y + cLen); ctx.lineTo(car.x, car.y); ctx.lineTo(car.x + cLen, car.y); ctx.stroke();
          ctx.beginPath(); ctx.moveTo(car.x + car.w - cLen, car.y); ctx.lineTo(car.x + car.w, car.y); ctx.lineTo(car.x + car.w, car.y + cLen); ctx.stroke();
          ctx.beginPath(); ctx.moveTo(car.x, car.y + car.h - cLen); ctx.lineTo(car.x, car.y + car.h); ctx.lineTo(car.x + cLen, car.y + car.h); ctx.stroke();
          ctx.beginPath(); ctx.moveTo(car.x + car.w - cLen, car.y + car.h); ctx.lineTo(car.x + car.w, car.y + car.h); ctx.lineTo(car.x + car.w, car.y + car.h - cLen); ctx.stroke();

          // Tag nhãn đỏ
          const tagH = 22;
          const tagW = Math.min(180, car.w + 24);
          const tagX = Math.max(4, car.x + (car.w - tagW) / 2);
          const tagY = Math.max(34, car.y - tagH - 4);

          ctx.fillStyle = '#ff0055';
          ctx.fillRect(tagX, tagY, tagW, tagH);
          ctx.fillStyle = '#ffffff';
          ctx.font = 'bold 11px monospace';
          ctx.textAlign = 'center';
          ctx.fillText(`🚨 XE VI PHẠM (LÀN ${car.lane})`, tagX + tagW / 2, tagY + 15);
        } else {
          // MẶC ĐỊNH THẤY XE KHÔNG VI PHẠM -> CHỈ KHOANH MÀU XANH, BỎ HOÀN TOÀN NHÃN "XE LÀN..."
          ctx.strokeStyle = '#00ff88';
          ctx.lineWidth = 2.5;
          ctx.strokeRect(car.x, car.y, car.w, car.h);

          // 4 góc bracket xanh
          const cLen = Math.min(14, car.w / 4);
          ctx.strokeStyle = '#00ff88';
          ctx.lineWidth = 2;
          ctx.beginPath(); ctx.moveTo(car.x, car.y + cLen); ctx.lineTo(car.x, car.y); ctx.lineTo(car.x + cLen, car.y); ctx.stroke();
          ctx.beginPath(); ctx.moveTo(car.x + car.w - cLen, car.y); ctx.lineTo(car.x + car.w, car.y); ctx.lineTo(car.x + car.w, car.y + cLen); ctx.stroke();
          ctx.beginPath(); ctx.moveTo(car.x, car.y + car.h - cLen); ctx.lineTo(car.x, car.y + car.h); ctx.lineTo(car.x + cLen, car.y + car.h); ctx.stroke();
          ctx.beginPath(); ctx.moveTo(car.x + car.w - cLen, car.y + car.h); ctx.lineTo(car.x + car.w, car.y + car.h); ctx.lineTo(car.x + car.w, car.y + car.h - cLen); ctx.stroke();
        }
      });

      // 3. Vẽ 2 vạch phân làn (nét đứt) tương ứng 2 vạch đen vật lý (chỉ để căn chỉnh)
      ctx.strokeStyle = 'rgba(0, 240, 255, 0.7)';
      ctx.lineWidth = 2;
      ctx.setLineDash([8, 6]);

      ctx.beginPath(); ctx.moveTo(x1, topY); ctx.lineTo(x1, bottomY); ctx.stroke();
      ctx.beginPath(); ctx.moveTo(x2, topY); ctx.lineTo(x2, bottomY); ctx.stroke();
      ctx.setLineDash([]);

      // 4. Nhãn tên từng làn ở đỉnh khung hình
      const headerH = 24;
      ctx.font = 'bold 11px monospace';
      ctx.textAlign = 'center';

      const lanes = [
        { name: 'LÀN 1 (TRÁI)', cx: x1 / 2, active: (hasSoundViolation && activeLane === 1), col: '#00ff88' },
        { name: 'LÀN 2 (GIỮA)', cx: x1 + (x2 - x1) / 2, active: (hasSoundViolation && activeLane === 2), col: '#ffb703' },
        { name: 'LÀN 3 (PHẢI)', cx: x2 + (w - x2) / 2, active: (hasSoundViolation && activeLane === 3), col: '#00f0ff' }
      ];

      lanes.forEach((lane, idx) => {
        const tagW = Math.min(105, (idx === 0 ? x1 : (idx === 1 ? x2 - x1 : w - x2)) - 8);
        const tagX = lane.cx - tagW / 2;
        const tagY = 6;

        ctx.fillStyle = lane.active ? 'rgba(255, 0, 85, 0.85)' : 'rgba(7, 11, 20, 0.75)';
        ctx.fillRect(tagX, tagY, tagW, headerH);

        ctx.strokeStyle = lane.active ? '#ff0055' : 'rgba(0, 240, 255, 0.4)';
        ctx.lineWidth = 1;
        ctx.strokeRect(tagX, tagY, tagW, headerH);

        ctx.fillStyle = lane.active ? '#ffffff' : lane.col;
        ctx.fillText(lane.active ? `🚨 ${lane.name}` : lane.name, lane.cx, tagY + headerH / 2 + 4);
      });

      ctx.restore();
    }

    function updateRotBtnLabel() {
      const el = document.getElementById('rotDeg');
      if (el) el.textContent = currentRotation + '°';
    }

    function cycleRotation() {
      const angles = [0, 90, 180, 270];
      let idx = angles.indexOf(currentRotation);
      currentRotation = angles[(idx + 1) % angles.length];
      localStorage.setItem('cam_rot', currentRotation);
      updateRotBtnLabel();
      renderPhotoToCanvas();
    }

    function renderPhotoToCanvas() {
      const camPhoto = document.getElementById('camPhoto');
      const canvas = document.getElementById('camCanvas');
      const placeholder = document.getElementById('camPhotoPlaceholder');
      if (!camPhoto || !camPhoto.naturalWidth) return;

      const isRot90 = (currentRotation === 90 || currentRotation === 270);
      canvas.width = isRot90 ? camPhoto.naturalHeight : camPhoto.naturalWidth;
      canvas.height = isRot90 ? camPhoto.naturalWidth : camPhoto.naturalHeight;
      const ctx = canvas.getContext('2d');

      ctx.clearRect(0, 0, canvas.width, canvas.height);
      ctx.save();
      ctx.translate(canvas.width / 2, canvas.height / 2);
      ctx.rotate((currentRotation * Math.PI) / 180);
      ctx.drawImage(camPhoto, -camPhoto.naturalWidth / 2, -camPhoto.naturalHeight / 2);
      ctx.restore();

      // Vẽ phân chia 3 làn ROI và khoanh đỏ xe vi phạm
      drawLaneOverlay(ctx, canvas.width, canvas.height, activeViolationLane, 0);

      // Cắt ảnh phóng to vùng đuôi xe / biển số sang khung thứ 2
      renderPlateCrop(canvas, lastDetectedVBox, activeViolationLane);

      canvas.style.display = 'block';
      if (placeholder) placeholder.style.display = 'none';
    }

    function renderPlateCrop(mainCanvas, vbox, laneId) {
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
        // Không vi phạm: Để trống hoàn toàn theo yêu cầu
        pctx.clearRect(0, 0, plateCanvas.width, plateCanvas.height);
        pctx.fillStyle = '#070b14';
        pctx.fillRect(0, 0, plateCanvas.width, plateCanvas.height);
        pctx.fillStyle = '#64748b';
        pctx.font = '12px monospace';
        pctx.textAlign = 'center';
        pctx.fillText('(Trống - Chỉ crop khi có xe vi phạm)', plateCanvas.width / 2, plateCanvas.height / 2);

        if (badge) {
          badge.textContent = 'TRỐNG';
          badge.style.color = '#64748b';
          badge.style.borderColor = 'rgba(100,116,139,0.3)';
          badge.style.background = 'transparent';
        }
        plateWrap.style.display = 'flex';
        return;
      }

      // Có xe vi phạm -> Cắt cận cảnh đuôi xe / biển số
      const cropY = Math.round(vbox.y + vbox.h * 0.25);
      const cropH = Math.round(vbox.h * 0.75);
      const cropX = Math.round(vbox.x);
      const cropW = Math.round(vbox.w);

      pctx.clearRect(0, 0, plateCanvas.width, plateCanvas.height);
      pctx.drawImage(mainCanvas, cropX, cropY, cropW, cropH, 0, 0, plateCanvas.width, plateCanvas.height);

      // Dải thông tin dưới ảnh crop vi phạm
      pctx.fillStyle = 'rgba(7, 11, 20, 0.85)';
      pctx.fillRect(0, plateCanvas.height - 24, plateCanvas.width, 24);
      pctx.fillStyle = '#ff0055';
      pctx.font = 'bold 11px monospace';
      pctx.textAlign = 'left';
      pctx.fillText(`🚨 BIỂN SỐ / ĐUÔI XE • LÀN ${vbox.lane}`, 10, plateCanvas.height - 8);

      pctx.fillStyle = '#ff0055';
      pctx.textAlign = 'right';
      pctx.fillText('VI PHẠM', plateCanvas.width - 10, plateCanvas.height - 8);

      if (badge) {
        badge.textContent = `LÀN ${vbox.lane} (VI PHẠM)`;
        badge.style.color = '#ff0055';
        badge.style.borderColor = 'rgba(255,0,85,0.4)';
        badge.style.background = 'rgba(255,0,85,0.2)';
      }
      plateWrap.style.display = 'flex';
    }

    function downloadPlateCrop() {
      const canvas = document.getElementById('plateCropCanvas');
      if (!canvas || !canvas.width) return;
      const a = document.createElement('a');
      const dtText = (lastEventData && lastEventData.dateTime) ? lastEventData.dateTime.replace(/[: ]/g, '_') : 'crop';
      a.download = `plate_crop_${dtText}.jpg`;
      a.href = canvas.toDataURL('image/jpeg', 0.95);
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

      // Vẽ DUY NHẤT ảnh gốc sạch đã xoay đúng chiều (KHÔNG VẼ VẠCH, KHÔNG CHỮ, KHÔNG BOX)
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

    function downloadStampedPhoto() {
      const camPhoto = document.getElementById('camPhoto');
      if (!camPhoto || !camPhoto.naturalWidth) return;

      const c = document.createElement('canvas');
      const isRot90 = (currentRotation === 90 || currentRotation === 270);
      c.width = isRot90 ? camPhoto.naturalHeight : camPhoto.naturalWidth;
      c.height = isRot90 ? camPhoto.naturalWidth : camPhoto.naturalHeight;
      const cctx = c.getContext('2d');

      // 1. Vẽ ảnh gốc đã xoay đúng chiều
      cctx.save();
      cctx.translate(c.width / 2, c.height / 2);
      cctx.rotate((currentRotation * Math.PI) / 180);
      cctx.drawImage(camPhoto, -camPhoto.naturalWidth / 2, -camPhoto.naturalHeight / 2);
      cctx.restore();

      // 2. Vẽ dải watermark đen mờ góc dưới
      const barH = Math.max(52, Math.round(c.height * 0.12));

      // Vẽ lớp phủ phân làn ROI & highlight làn vi phạm (đóng dấu vào ảnh để làm dataset)
      drawLaneOverlay(cctx, c.width, c.height, activeViolationLane, barH);

      cctx.fillStyle = 'rgba(0, 0, 0, 0.75)';
      cctx.fillRect(0, c.height - barH, c.width, barH);

      // Vạch viền cyan trên thanh watermark
      cctx.fillStyle = '#00f0ff';
      cctx.fillRect(0, c.height - barH, c.width, 3);

      // 3. Vẽ chữ ngày giờ & thông số vi phạm
      const fontSize = Math.max(15, Math.round(barH * 0.36));
      cctx.font = `bold ${fontSize}px monospace`;
      cctx.fillStyle = '#00f0ff';
      const dtText = lastEventData ? lastEventData.dateTime : new Date().toLocaleString('vi-VN');
      cctx.fillText(dtText, 16, c.height - barH * 0.52);

      cctx.font = `${Math.round(fontSize * 0.85)}px monospace`;
      cctx.fillStyle = '#ffb703';
      const infoText = lastEventData ? `${lastEventData.db} dB | ${lastEventData.lane} (${lastEventData.pos} cm)` : '';
      cctx.fillText(infoText, 16, c.height - barH * 0.18);

      // 4. Tạo link tải file
      const a = document.createElement('a');
      a.download = `noise_osd_${dtText.replace(/[: ]/g, '_')}.jpg`;
      a.href = c.toDataURL('image/jpeg', 0.95);
      a.click();
    }

    function triggerTestCapture() {
      activeViolationLane = 0; // Đang test, không có vi phạm âm thanh
      const photoTimeTag = document.getElementById('photoTimeTag');
      photoTimeTag.textContent = 'Đang ra lệnh chụp thử...';
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ cmd: "capture" }));
      } else {
        alert('Chưa kết nối WebSocket với ESP32!');
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
        // Gửi thời gian thực từ trình duyệt xuống ESP32
        ws.send(JSON.stringify({ cmd: "sync_time", time: Math.floor(Date.now() / 1000) }));

        // Kiểm tra xem đã có ảnh lưu sẵn trên ESP32 chưa
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
              if (btnDlDirect) btnDlDirect.style.display = 'inline-block';
              if (btnDl) btnDl.style.display = 'inline-block';
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

            // Cập nhật thanh VU meter (dải chuẩn dBSPL: 0 - 120 dB)
            const m1Pct = Math.min(100, Math.max(0, (m1 / 120.0) * 100));
            const m2Pct = Math.min(100, Math.max(0, (m2 / 120.0) * 100));
            m1Bar.style.width = m1Pct + '%';
            m2Bar.style.width = m2Pct + '%';
          }

          if (d.event) {
            const ev = d.event;
            eventDbEl.innerHTML = ev.db.toFixed(1) + ' <span style="font-size: 1rem; color: var(--dim);">dB</span>';
            eventPosEl.innerHTML = ev.pos.toFixed(1) + ' <span style="font-size: 1rem; color: var(--dim);">cm</span>';
            eventTdoaEl.innerHTML = (ev.tdoa > 0 ? '+' : '') + ev.tdoa.toFixed(0) + ' <span style="font-size: 1rem; color: var(--dim);">μs</span>';

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

            eventStatusEl.textContent = "ĐÃ BẮT TIẾNG ỒN VƯỢT NGƯỠNG (TB 30 LẦN)";
            eventStatusEl.style.color = "var(--danger)";
            eventLaneDesc.textContent = laneName;

            alertBanner.style.display = 'flex';
            alertMsg.textContent = `⚠️ PHÁT HIỆN TIẾNG ỒN ${ev.db.toFixed(1)} dB TẠI VỊ TRÍ ${ev.pos.toFixed(1)} cm!`;
            alertLane.textContent = laneName;
            alertLane.style.background = laneClass;
            alertLane.style.color = '#000';

            addHistoryRow(ev);

            // Tạo chuỗi ngày giờ đầy đủ (YYYY-MM-DD HH:mm:ss)
            const now = new Date();
            const dateStr = now.getFullYear() + '-' +
                            String(now.getMonth() + 1).padStart(2, '0') + '-' +
                            String(now.getDate()).padStart(2, '0');
            const timeStr = String(now.getHours()).padStart(2, '0') + ':' +
                            String(now.getMinutes()).padStart(2, '0') + ':' +
                            String(now.getSeconds()).padStart(2, '0');
            const fullDateTime = dateStr + ' ' + timeStr;

            lastEventData = {
              dateTime: fullDateTime,
              db: ev.db.toFixed(1),
              pos: ev.pos.toFixed(1),
              lane: laneName
            };

            // Hiển thị phần Audio và báo đang thu 1.5s tiếp theo
            const audioSection = document.getElementById('audioSection');
            const audioStatus = document.getElementById('audioStatus');
            if (audioSection) audioSection.style.display = 'flex';
            if (audioStatus) {
              audioStatus.textContent = 'Đang thu 1.5s sau tiếng ồn...';
              audioStatus.style.color = 'var(--warn)';
            }

            // Cập nhật ảnh chụp từ Camera và nhãn OSD ngày giờ
            const camPhoto = document.getElementById('camPhoto');
            const camPhotoPlaceholder = document.getElementById('camPhotoPlaceholder');
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
              btnDownloadPhoto.style.display = 'inline-block';
              if (btnDownloadPhotoDirect) btnDownloadPhotoDirect.style.display = 'inline-block';
              photoTimeTag.textContent = fullDateTime;
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
            if (btnDownloadAudio) btnDownloadAudio.style.display = 'inline-block';
            if (audioStatus) {
              audioStatus.textContent = 'Sẵn sàng nghe (3s WAV)';
              audioStatus.style.color = 'var(--green)';
            }
          }

          if (d.cam_ok) {
            const camPhoto = document.getElementById('camPhoto');
            const camPhotoPlaceholder = document.getElementById('camPhotoPlaceholder');
            const photoTimeTag = document.getElementById('photoTimeTag');
            const btnDownloadPhoto = document.getElementById('btnDownloadPhoto');
            const btnDownloadPhotoDirect = document.getElementById('btnDownloadPhotoDirect');
            const now = new Date();
            const dateStr = now.getFullYear() + '-' +
                            String(now.getMonth() + 1).padStart(2, '0') + '-' +
                            String(now.getDate()).padStart(2, '0');
            const timeStr = String(now.getHours()).padStart(2, '0') + ':' +
                            String(now.getMinutes()).padStart(2, '0') + ':' +
                            String(now.getSeconds()).padStart(2, '0');
            const fullDateTime = dateStr + ' ' + timeStr;
            lastEventData = { dateTime: fullDateTime, db: 'TEST', pos: '0.0', lane: 'CHỤP THỬ' };
            activeViolationLane = 0; // Chụp thử: Không có sự kiện vi phạm âm thanh
            camPhoto.src = '/photo.jpg?t=' + Date.now();
            camPhoto.onload = () => {
              renderPhotoToCanvas();
              btnDownloadPhoto.style.display = 'inline-block';
              if (btnDownloadPhotoDirect) btnDownloadPhotoDirect.style.display = 'inline-block';
              photoTimeTag.textContent = 'Chụp thử thành công: ' + fullDateTime;
            };
          }

          if (d.cam_err) {
            const photoTimeTag = document.getElementById('photoTimeTag');
            photoTimeTag.textContent = 'Lỗi camera: ' + d.code;
            alertBanner.style.display = 'flex';
            alertMsg.textContent = '⚠️ LỖI KHỞI ĐỘNG CAMERA (' + d.code + '): Kiểm tra cáp cắm FPC của camera!';
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

    updateRotBtnLabel();
    updateROIBtnLabel();
    connectWs();
    syncSettingsToWs();
  </script>
</body>
</html>
)rawliteral";
