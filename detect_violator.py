"""
detect_violator.py: Module xử lý AI thị giác máy tính phát hiện xe ở làn vượt ngưỡng tiếng ồn
Kết hợp: Mô hình YOLOv8n đã huấn luyện (models/best.pt) + Âm thanh TDoA (Làn 1, 2, 3)
"""

import os
import glob
import cv2
import numpy as np

# Ranh giới 3 làn (khớp config.h của ESP32)
LANE_1_MAX_PCT = 35.0  # Làn 1: 0 - 35%
LANE_2_MAX_PCT = 68.0  # Làn 2: 35 - 68%, Làn 3: 68 - 100%
ROAD_TOP_Y_RATIO = 0.28 # Đỉnh lòng đường ~ 28% chiều cao ảnh (180/640)

# Tải mô hình YOLOv8n nếu có
YOLO_MODEL = None
MODEL_PATH = "models/best.pt"
if os.path.exists(MODEL_PATH):
    try:
        from ultralytics import YOLO
        YOLO_MODEL = YOLO(MODEL_PATH)
        print(f"[AI] Đã tải thành công mô hình YOLO tùy biến: {MODEL_PATH}")
    except Exception as e:
        print(f"[Cảnh báo] Lỗi tải YOLO: {e}, sẽ chuyển sang trừ nền.")

def get_lane_id(x_center, img_width):
    """Xác định làn đường dựa trên tọa độ tâm X của xe"""
    pct = (x_center / float(img_width)) * 100.0
    if pct < LANE_1_MAX_PCT:
        return 1, "LAN 1 (TRAI)", pct
    elif pct > LANE_2_MAX_PCT:
        return 3, "LAN 3 (PHAI)", pct
    else:
        return 2, "LAN 2 (GIUA)", pct

def detect_vehicles(img, conf_thresh=0.35):
    """
    Phát hiện xe bằng mô hình YOLOv8n tốt nhất.
    """
    h, w = img.shape[:2]
    vehicles = []

    if YOLO_MODEL is not None:
        results = YOLO_MODEL.predict(img, conf=conf_thresh, imgsz=320, verbose=False)[0]
        boxes = results.boxes
        for box in boxes:
            xyxy = box.xyxy[0].cpu().numpy()
            conf = float(box.conf[0].cpu().numpy())
            bx1, by1, bx2, by2 = int(xyxy[0]), int(xyxy[1]), int(xyxy[2]), int(xyxy[3])
            bw = bx2 - bx1
            bh = by2 - by1

            cx = bx1 + bw / 2.0
            cy = by1 + bh / 2.0

            lane_id, lane_name, pct = get_lane_id(cx, w)
            vehicles.append({
                "bbox": (bx1, by1, bw, bh),
                "center": (cx, cy),
                "conf": conf,
                "lane_id": lane_id,
                "lane_name": lane_name,
                "pct": pct
            })
    return vehicles

def draw_hud_and_fusion(img, vehicles, violation_lane, violation_db=88.5):
    """
    Vẽ lớp phủ HUD phân tích vi phạm và cắt ảnh vùng xe/biển số
    """
    out = img.copy()
    h, w = out.shape[:2]
    road_top_y = int(h * ROAD_TOP_Y_RATIO)

    x1 = int(w * (LANE_1_MAX_PCT / 100.0))
    x2 = int(w * (LANE_2_MAX_PCT / 100.0))

    # 1. Vẽ 2 vạch phân làn (cyan nét đứt)
    for vx in [x1, x2]:
        for y in range(road_top_y, h, 20):
            cv2.line(out, (vx, y), (vx, min(y + 12, h)), (255, 240, 0), 2)

    # 2. Nhãn 3 làn đường
    lane_headers = [
        ("LAN 1 (TRAI)", x1 // 2, (0, 255, 136)),
        ("LAN 2 (GIUA)", x1 + (x2 - x1) // 2, (3, 183, 255)),
        ("LAN 3 (PHAI)", x2 + (w - x2) // 2, (255, 240, 0))
    ]
    for name, cx, col in lane_headers:
        cv2.rectangle(out, (cx - 55, road_top_y - 26), (cx + 55, road_top_y - 4), (20, 15, 7), -1)
        cv2.rectangle(out, (cx - 55, road_top_y - 26), (cx + 55, road_top_y - 4), col, 1)
        cv2.putText(out, name, (cx - 48, road_top_y - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.42, col, 1, cv2.LINE_AA)

    # 3. So khớp xe với làn vi phạm TDoA (Fusion Decision)
    has_match = False
    violator_crop = None

    for v in vehicles:
        x, y, bw, bh = v["bbox"]
        lane_id = v["lane_id"]
        conf = v.get("conf", 1.0)

        if lane_id == violation_lane:
            # Khớp: Xe này vi phạm
            has_match = True
            box_col = (0, 0, 255)  # Đỏ
            label = f"[VI PHAM] L{lane_id} ({conf*100:.0f}%) | {violation_db:.1f}dB"

            # Tint đỏ làn vi phạm
            rx = 0 if lane_id == 1 else (x1 if lane_id == 2 else x2)
            rw = x1 if lane_id == 1 else ((x2 - x1) if lane_id == 2 else (w - x2))
            overlay = out.copy()
            cv2.rectangle(overlay, (rx, road_top_y), (rx + rw, h), (0, 0, 255), -1)
            cv2.addWeighted(overlay, 0.20, out, 0.80, 0, out)

            # Khung xe vi phạm
            cv2.rectangle(out, (x, y), (x + bw, y + bh), box_col, 3)
            cv2.rectangle(out, (x, max(0, y - 24)), (x + bw, y), box_col, -1)
            cv2.putText(out, label, (x + 4, y - 7),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 255), 1, cv2.LINE_AA)

            # Cắt ảnh vùng đuôi xe (biển số)
            cy1 = max(0, y)
            cy2 = min(h, y + bh)
            cx1 = max(0, x)
            cx2 = min(w, x + bw)
            violator_crop = img[cy1:cy2, cx1:cx2]
        else:
            # Xe hợp lệ
            box_col = (0, 255, 136)
            label = f"[HOP LE] L{lane_id} ({conf*100:.0f}%)"
            cv2.rectangle(out, (x, y), (x + bw, y + bh), box_col, 2)
            cv2.rectangle(out, (x, max(0, y - 20)), (x + bw, y), (20, 15, 7), -1)
            cv2.rectangle(out, (x, max(0, y - 20)), (x + bw, y), box_col, 1)
            cv2.putText(out, label, (x + 4, y - 6),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.40, box_col, 1, cv2.LINE_AA)

    # 4. Banner HUD Header
    banner_h = 56
    cv2.rectangle(out, (0, 0), (w, banner_h), (15, 10, 7), -1)
    cv2.line(out, (0, banner_h), (w, banner_h), (0, 240, 255), 2)

    status_str = f"TDoA LAN VI PHAM: LAN {violation_lane} ({violation_db:.1f} dB)"
    if has_match:
        res_str = f">> KET QUA: [KHOP] - XE LAN {violation_lane} VI PHAM TIENG ON!"
        res_col = (0, 60, 255)
    else:
        res_str = f">> KET QUA: [KHONG KHOP] - KHONG CO XE LAN {violation_lane} (NHIEU)"
        res_col = (0, 215, 255)

    cv2.putText(out, status_str, (12, 22), cv2.FONT_HERSHEY_SIMPLEX, 0.52, (255, 255, 255), 1, cv2.LINE_AA)
    cv2.putText(out, res_str, (12, 45), cv2.FONT_HERSHEY_SIMPLEX, 0.50, res_col, 2, cv2.LINE_AA)

    return out, has_match, violator_crop

def test_inference_on_dataset():
    out_dir = "output"
    os.makedirs(out_dir, exist_ok=True)

    # Lấy các mẫu ảnh đại diện từ dataset: 1 empty, 1 lane1, 1 lane2, 1 lane3, 1 multi
    sample_tests = [
        ("dataset/images/empty_004_20261116_123407.jpg", 2, 85.0, "Đường trống (TDoA báo Làn 2 -> Kỳ vọng: Không khớp)"),
        ("dataset/images/lane1_031_20261116_123749.jpg", 1, 91.2, "Xe Làn 1 (TDoA báo Làn 1 -> Kỳ vọng: Khớp xe làn 1)"),
        ("dataset/images/lane2_018_20261116_123503.jpg", 2, 94.5, "Xe Làn 2 (TDoA báo Làn 2 -> Kỳ vọng: Khớp xe làn 2)"),
        ("dataset/images/lane3_050_20261116_124008.jpg", 3, 89.8, "Xe Làn 3 (TDoA báo Làn 3 -> Kỳ vọng: Khớp xe làn 3)"),
        ("dataset/images/multi_058_20261116_124050.jpg", 2, 96.0, "Nhiều xe (TDoA báo Làn 2 -> Kỳ vọng: Chỉ phạt xe làn 2)")
    ]

    print("\n" + "="*60)
    print("      CHẠY ĐÁNH GIÁ MÔ HÌNH YOLOv8n VỚI DỮ LIỆU THỰC TẾ")
    print("="*60)

    for idx, (img_path, lane, db, desc) in enumerate(sample_tests):
        if not os.path.exists(img_path):
            continue
        img = cv2.imread(img_path)
        base = os.path.basename(img_path)

        vehicles = detect_vehicles(img)
        res_img, match, crop = draw_hud_and_fusion(img, vehicles, violation_lane=lane, violation_db=db)

        # Lưu ảnh kết quả và ảnh crop xe
        res_name = f"eval_{idx+1}_{base}"
        cv2.imwrite(os.path.join(out_dir, res_name), res_img)
        if crop is not None and crop.size > 0:
            crop_name = f"crop_plate_{idx+1}_{base}"
            cv2.imwrite(os.path.join(out_dir, crop_name), crop)

        eval_str = "[KHỚP]" if match else "[KHÔNG KHỚP]"
        print(f"\n[Mẫu {idx+1}] {base}")
        print(f"  • Kịch bản: {desc}")
        print(f"  • YOLO phát hiện: {len(vehicles)} xe")
        for v in vehicles:
            print(f"      - {v['lane_name']}: BBox={v['bbox']}, Conf={v['conf']*100:.1f}%, X_pct={v['pct']:.1f}%")
        print(f"  • Quyết định Fusion: {eval_str}")
        print(f"  • Lưu kết quả: output/{res_name}")

if __name__ == "__main__":
    test_inference_on_dataset()
