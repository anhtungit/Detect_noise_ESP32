import os
import glob
import cv2
import numpy as np

def generate_all_labels():
    img_dir = "dataset/images"
    lbl_dir = "dataset/labels"
    os.makedirs(lbl_dir, exist_ok=True)

    # 1. Tạo median background từ 12 ảnh empty
    empty_files = sorted(glob.glob(os.path.join(img_dir, "empty_*.jpg")))
    if not empty_files:
        print("[Lỗi] Không tìm thấy ảnh empty_* để làm nền!")
        return

    print(f"Xây dựng ảnh nền sạch từ {len(empty_files)} ảnh empty...")
    bg_stack = [cv2.imread(f) for f in empty_files]
    median_bg = np.median(bg_stack, axis=0).astype(np.uint8)
    median_bg_gray = cv2.cvtColor(median_bg, cv2.COLOR_BGR2GRAY)

    all_images = sorted(glob.glob(os.path.join(img_dir, "*.jpg")))
    print(f"Bắt đầu gán nhãn cho {len(all_images)} ảnh...")

    total_boxes = 0

    for img_path in all_images:
        base_name = os.path.splitext(os.path.basename(img_path))[0]
        lbl_path = os.path.join(lbl_dir, f"{base_name}.txt")

        # Nếu là ảnh empty -> tạo file nhãn rỗng (negative sample)
        if base_name.startswith("empty_"):
            with open(lbl_path, "w", encoding="utf-8") as f:
                pass
            continue

        img = cv2.imread(img_path)
        if img is None:
            continue
        h, w = img.shape[:2]

        # Trừ nền kết hợp lọc màu HSV để loại bỏ bóng đổ (shadow) trên nền trắng
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        diff = cv2.absdiff(median_bg_gray, gray)

        # Chuyển sang HSV để phát hiện độ bão hòa màu và bóng đổ
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        sat = hsv[:, :, 1]
        val = hsv[:, :, 2]

        # Mặt đường tính từ y = 180 trở xuống, bỏ viền ngoài
        road_mask = np.zeros((h, w), dtype=np.uint8)
        road_mask[180:h-10, 15:w-15] = 255
        diff = cv2.bitwise_and(diff, diff, mask=road_mask)

        # Lọc ngưỡng: Điểm ảnh xe phải có diff lớn HOẶC độ bão hòa màu cao (khử bóng mờ nhạt trên giấy trắng)
        # Bóng đổ trên nền trắng: diff ~ 20-40 nhưng sat rất thấp (< 35)
        is_car = (diff > 42) | ((diff > 25) & (sat > 45))
        thresh = np.zeros_like(diff, dtype=np.uint8)
        thresh[is_car & (road_mask > 0)] = 255

        # Lọc nhiễu: Kernel nhỏ (5x5) 1 lần close, 1 lần open để không nối bóng đổ vào thân xe
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))
        thresh = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel, iterations=1)
        thresh = cv2.morphologyEx(thresh, cv2.MORPH_CLOSE, kernel, iterations=1)

        cnts, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        boxes = []
        for c in cnts:
            area = cv2.contourArea(c)
            # Xe mô hình tối thiểu ~2000px
            if area > 2000:
                bx, by, bw, bh = cv2.boundingRect(c)

                # Lọc tỉ lệ khung hình: Xe thật không thể cao quá 2.8 lần rộng (tránh nuốt vệt bóng dài)
                aspect_ratio = bh / float(bw)
                if aspect_ratio > 2.8 or aspect_ratio < 0.4:
                    continue

                # Bỏ qua nếu vùng nằm sát mép trên không phải lòng đường
                if by < 170:
                    continue

                boxes.append((bx, by, bw, bh))

        # Ghi file nhãn YOLO
        with open(lbl_path, "w", encoding="utf-8") as lf:
            for bx, by, bw, bh in boxes:
                xc = (bx + bw / 2.0) / w
                yc = (by + bh / 2.0) / h
                nw = bw / float(w)
                nh = bh / float(h)
                lf.write(f"0 {xc:.6f} {yc:.6f} {nw:.6f} {nh:.6f}\n")
                total_boxes += 1

        print(f"  • {base_name}.jpg -> {len(boxes)} xe")

    print(f"\nHoàn tất! Tổng cộng {total_boxes} nhãn xe được tạo trong: {lbl_dir}/")

if __name__ == "__main__":
    generate_all_labels()
