# ESP32-S3 Road Noise Localization & Auto Capture

Hệ thống định vị tiếng ồn trên làn đường (2x Micro INMP441) kết hợp tự động chụp ảnh phương tiện (Camera OV3660/OV5640), ghi âm bằng chứng 3 giây (WAV) và hiển thị Web Dashboard realtime qua Wi-Fi AP.

---

## 🔌 Sơ Đồ Đấu Nối (ESP32-S3 N16R8 Cam Board)

### 1. Micro I2S INMP441 (Dùng chung 1 bus)
| Chân Mic | ESP32-S3 | Ghi chú |
| :--- | :--- | :--- |
| **SCK / BCLK** | **GPIO 1** | Chung 2 mic |
| **WS / LRCK** | **GPIO 2** | Chung 2 mic |
| **SD / DOUT** | **GPIO 42** | Chung 2 mic |
| **L/R (Mic 1)** | **GND** | Mic 1 - Lề trái ($0\,\text{cm}$) |
| **L/R (Mic 2)** | **3.3V** | Mic 2 - Lề phải ($W\,\text{cm}$) |
| **VDD / GND** | **3.3V / GND** | Nguồn |

### 2. Camera DVP (Tích hợp sẵn trên board)
* **XCLK:** 15 | **PCLK:** 13 | **VSYNC:** 6 | **HREF:** 7
* **SDA (SIOD):** 4 | **SCL (SIOC):** 5
* **Data (D0..D7):** 11, 9, 8, 10, 12, 18, 17, 16

---

## ⚡ Nguyên Lý Hoạt Động

1. **Lấy mẫu:** I2S Stereo 48kHz, tính mức dBSPL chuẩn datasheet.
2. **Định vị TDoA:** Tương quan chéo (Cross-Correlation) tìm độ trễ sóng âm $\Delta t \implies d_1 = (W - v \cdot \Delta t)/2$.
3. **Lọc nhiễu:** Bắt đủ 30 mẫu vượt ngưỡng mới tính trung bình để kích hoạt.
4. **Bằng chứng vi phạm:**
   * **Chụp ảnh:** Chụp frame VGA ($640 \times 480$), đóng dấu ngày giờ OSD.
   * **Ghi âm 3 giây:** Ring Buffer lưu $1.5\,\text{s}$ trước $+ 1.5\,\text{s}$ sau tiếng ồn đóng gói file WAV 48kHz.

---

## 🚀 Hướng Dẫn Chạy

1. **Nạp Firmware:**
   ```powershell
   pio run -e esp32-s3-cam --target upload
   ```
2. **Kết Nối Wi-Fi:**
   * **SSID:** `ESP32-Audio-Spectrum`
   * **Mật khẩu:** `12345678`
3. **Mở Dashboard:**
   * Truy cập: `http://192.168.4.1` (hoặc `http://esp32.local`).
   * Xem ảnh có đóng dấu ngày giờ, nghe lại đoạn ghi âm 3s và tải file WAV bằng chứng.
