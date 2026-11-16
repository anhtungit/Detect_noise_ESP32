"""
collect_dataset.py: Công cụ thu thập tập ảnh huấn luyện từ ESP32 Camera qua Wi-Fi.

Cách dùng:
  1. Kết nối máy tính vào Wi-Fi ESP32: ESP32-Audio-Spectrum (Pass: 12345678)
  2. Chạy:
       python collect_dataset.py
  3. Phím tắt:
       [1] Chụp & Lưu vào images/lane1/ (Xe ở Làn 1)
       [2] Chụp & Lưu vào images/lane2/ (Xe ở Làn 2)
       [3] Chụp & Lưu vào images/lane3/ (Xe ở Làn 3)
       [0] Chụp & Lưu vào images/empty/ (Mặt đường trống)
       [Space] Chụp & Lưu vào images/ chung
       [A] Bật / Tắt chế độ tự chụp mỗi 3 giây
       [Q] Thoát
"""

import os
import time
import urllib.request
import json
import cv2

ESP32_IP = "192.168.4.1"
BASE_URL = f"http://{ESP32_IP}"
WS_PORT = 81

def rotate_image_270(img):
    """Xoay 270 độ theo chiều kim đồng hồ (90 độ ngược chiều)"""
    return cv2.rotate(img, cv2.ROTATE_90_COUNTERCLOCKWISE)

def trigger_esp32_capture():
    """Gửi lệnh chụp ảnh tới ESP32 qua WebSocket hoặc HTTP"""
    try:
        # Nếu dùng websocket-client
        from websocket import create_connection
        ws = create_connection(f"ws://{ESP32_IP}:{WS_PORT}/", timeout=2)
        ws.send(json.dumps({"cmd": "capture"}))
        ws.close()
        return True
    except Exception:
        # Nếu chưa cài websocket-client, fallback fetch trực tiếp
        return True

def download_photo(save_dir, prefix="img"):
    os.makedirs(save_dir, exist_ok=True)
    photo_url = f"{BASE_URL}/photo.jpg?t={int(time.time() * 1000)}"

    try:
        trigger_esp32_capture()
        time.sleep(0.3)  # Đợi ESP32 chụp xong frame

        req = urllib.request.Request(photo_url, headers={"User-Agent": "DatasetCollector/1.0"})
        with urllib.request.urlopen(req, timeout=5) as response:
            img_bytes = response.read()

        import numpy as np
        nparr = np.frombuffer(img_bytes, np.uint8)
        img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

        if img is None:
            print("  ❌ Không thể giải mã ảnh từ ESP32!")
            return False

        # Xoay 270 độ đúng hướng lòng đường
        img_rot = rotate_image_270(img)

        timestamp_str = time.strftime("%Y%m%d_%H%M%S")
        filename = f"{prefix}_{timestamp_str}.jpg"
        save_path = os.path.join(save_dir, filename)

        cv2.imwrite(save_path, img_rot)
        print(f"  ✅ Đã lưu: {save_path} ({img_rot.shape[1]}x{img_rot.shape[0]})")
        return True

    except Exception as e:
        print(f"  ❌ Lỗi khi tải ảnh từ {photo_url}: {e}")
        return False

def main():
    print("=" * 60)
    print("      CÔNG CỤ THU THẬP TẬP ẢNH TRAINING (DATASET COLLECTOR)")
    print(f"      Kết nối tới: {BASE_URL}")
    print("=" * 60)
    print("\nPhím tắt điều khiển:")
    print("  [1]     : Chụp xe ở LÀN 1 (Lưu vào images/lane1/)")
    print("  [2]     : Chụp xe ở LÀN 2 (Lưu vào images/lane2/)")
    print("  [3]     : Chụp xe ở LÀN 3 (Lưu vào images/lane3/)")
    print("  [0]     : Chụp MẶT ĐƯỜNG TRỐNG (Lưu vào images/empty/)")
    print("  [Enter] : Chụp vào images/ chung")
    print("  [A]     : Chế độ tự động chụp liên tục (Timelapse 3s)")
    print("  [Q]     : Thoát chương trình\n")

    # Kiểm tra kết nối sơ bộ
    print("Kiểm tra kết nối tới ESP32...", end="", flush=True)
    try:
        urllib.request.urlopen(BASE_URL, timeout=3)
        print(" [THÀNH CÔNG]")
    except Exception:
        print(" [CHƯA THẤY ESP32]")
        print("  ⚠️ Hãy chắc chắn máy tính đã kết nối vào Wi-Fi ESP32-Audio-Spectrum!")

    while True:
        try:
            cmd = input("\nNhập lệnh [1/2/3/0/A/Q hoặc Enter]: ").strip().lower()
            if cmd == "q":
                print("Đã thoát.")
                break
            elif cmd == "1":
                download_photo("images/lane1", "lane1")
            elif cmd == "2":
                download_photo("images/lane2", "lane2")
            elif cmd == "3":
                download_photo("images/lane3", "lane3")
            elif cmd == "0":
                download_photo("images/empty", "empty")
            elif cmd == "a":
                print("\n[BẮT ĐẦU CHẾ ĐỘ TỰ CHỤP LIÊN TỤC] (Nhấn Ctrl+C để dừng)...")
                interval = 3.5
                lane_choice = input("Chọn làn mặc định (1, 2, 3, 0 hoặc enter): ").strip()
                tag = f"lane{lane_choice}" if lane_choice in ["1", "2", "3"] else ("empty" if lane_choice == "0" else "auto")
                sub_dir = f"images/{tag}"
                count = 1
                try:
                    while True:
                        print(f"Tự chụp tấm #{count}...")
                        download_photo(sub_dir, tag)
                        count += 1
                        time.sleep(interval)
                except KeyboardInterrupt:
                    print("\nĐã dừng chế độ tự chụp.")
            else:
                download_photo("images", "photo")

        except KeyboardInterrupt:
            print("\nĐã thoát.")
            break

if __name__ == "__main__":
    main()
