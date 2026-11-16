import os
import glob
import shutil
import random
from ultralytics import YOLO

def setup_train_val_split():
    dataset_root = "dataset"
    src_img_dir = os.path.join(dataset_root, "images")
    src_lbl_dir = os.path.join(dataset_root, "labels")

    train_img_dir = os.path.join(dataset_root, "train", "images")
    train_lbl_dir = os.path.join(dataset_root, "train", "labels")
    val_img_dir = os.path.join(dataset_root, "val", "images")
    val_lbl_dir = os.path.join(dataset_root, "val", "labels")

    for d in [train_img_dir, train_lbl_dir, val_img_dir, val_lbl_dir]:
        os.makedirs(d, exist_ok=True)
        # Xóa file cũ nếu có
        for f in glob.glob(os.path.join(d, "*")):
            if os.path.isfile(f):
                os.remove(f)

    all_images = sorted(glob.glob(os.path.join(src_img_dir, "*.jpg")))
    random.seed(42)
    random.shuffle(all_images)

    # 80% train, 20% val
    val_count = max(8, int(len(all_images) * 0.2))
    val_set = set(all_images[:val_count])

    print(f"Tổng số ảnh: {len(all_images)} -> Train: {len(all_images) - val_count}, Val: {val_count}")

    for img_path in all_images:
        base_name = os.path.splitext(os.path.basename(img_path))[0]
        lbl_path = os.path.join(src_lbl_dir, f"{base_name}.txt")

        if img_path in val_set:
            shutil.copy(img_path, val_img_dir)
            if os.path.exists(lbl_path):
                shutil.copy(lbl_path, val_lbl_dir)
        else:
            shutil.copy(img_path, train_img_dir)
            if os.path.exists(lbl_path):
                shutil.copy(lbl_path, train_lbl_dir)

    # Tạo data.yaml
    abs_root = os.path.abspath(dataset_root).replace("\\", "/")
    yaml_content = f"""path: {abs_root}
train: train/images
val: val/images

names:
  0: vehicle
"""
    yaml_path = os.path.join(dataset_root, "data.yaml")
    with open(yaml_path, "w", encoding="utf-8") as yf:
        yf.write(yaml_content)

    print(f"Đã tạo file cấu hình: {yaml_path}")
    return yaml_path

def start_training(yaml_path):
    print("\n" + "=" * 60)
    print("          BẮT ĐẦU HUẤN LUYỆN YOLOV8 NANO TRÊN TẬP ẢNH MỚI")
    print("=" * 60)

    # Khởi tạo mô hình YOLOv8 Nano pretrained
    model = YOLO("yolov8n.pt")

    # Huấn luyện 25 epochs, ảnh 320x320
    results = model.train(
        data=yaml_path,
        epochs=25,
        imgsz=320,
        batch=8,
        workers=0,
        device="cpu",
        project="runs/detect",
        name="road_vehicle_model",
        exist_ok=True,
        verbose=True
    )

    best_model_path = "runs/detect/road_vehicle_model/weights/best.pt"
    print("\n" + "=" * 60)
    print("              HUẤN LUYỆN HOÀN TẤT THÀNH CÔNG!")
    print(f"  • Trọng số mô hình tốt nhất lưu tại: {best_model_path}")
    print("=" * 60)
    return best_model_path

if __name__ == "__main__":
    yaml_path = setup_train_val_split()
    start_training(yaml_path)
