"""
export_yolo_dataset.py: Tự động trích xuất Bounding Box từ 5 ảnh mẫu thành dataset chuẩn YOLO
Cấu trúc:
  dataset/
    images/
    labels/
    data.yaml
"""

import os
import glob
import cv2
import shutil
from detect_violator import detect_vehicles

def export_dataset():
    dataset_dir = "dataset"
    img_out_dir = os.path.join(dataset_dir, "images")
    lbl_out_dir = os.path.join(dataset_dir, "labels")

    os.makedirs(img_out_dir, exist_ok=True)
    os.makedirs(lbl_out_dir, exist_ok=True)

    files = sorted(glob.glob("images/*.jpg"))
    print(f"Bắt đầu xuất nhãn YOLO cho {len(files)} ảnh...")

    total_boxes = 0

    for idx, f in enumerate(files):
        img = cv2.imread(f)
        if img is None:
            continue
        h, w = img.shape[:2]
        base_name = os.path.splitext(os.path.basename(f))[0]

        # 1. Copy ảnh vào dataset/images
        dest_img_path = os.path.join(img_out_dir, f"{base_name}.jpg")
        shutil.copyfile(f, dest_img_path)

        # 2. Phát hiện xe
        vehicles = detect_vehicles(img)

        # 3. Ghi file nhãn .txt chuẩn YOLO
        label_file = os.path.join(lbl_out_dir, f"{base_name}.txt")
        with open(label_file, "w", encoding="utf-8") as lf:
            for v in vehicles:
                x, y, bw, bh = v["bbox"]
                # Chuyển đổi sang tọa độ chuẩn hóa YOLO [0..1]
                x_center = (x + bw / 2.0) / w
                y_center = (y + bh / 2.0) / h
                norm_w = bw / float(w)
                norm_h = bh / float(h)

                # Class 0: vehicle
                lf.write(f"0 {x_center:.6f} {y_center:.6f} {norm_w:.6f} {norm_h:.6f}\n")
                total_boxes += 1

        print(f"  • {base_name}.jpg -> {len(vehicles)} nhãn xe ({label_file})")

    # 4. Tạo file cấu hình data.yaml cho YOLOv8/YOLO11
    abs_path = os.path.abspath(dataset_dir).replace('\\', '/')
    yaml_content = f"""# Cấu hình Dataset YOLO nhận diện xe trên mô hình 3 làn
path: {abs_path}
train: images
val: images

names:
  0: vehicle
"""
    yaml_path = os.path.join(dataset_dir, "data.yaml")
    with open(yaml_path, "w", encoding="utf-8") as yf:
        yf.write(yaml_content)

    print("\n=======================================================")
    print(f"Đã xuất thành công {total_boxes} nhãn xe vào: {dataset_dir}/")
    print(f"File cấu hình YOLO: {yaml_path}")
    print("=======================================================")

if __name__ == "__main__":
    export_dataset()
