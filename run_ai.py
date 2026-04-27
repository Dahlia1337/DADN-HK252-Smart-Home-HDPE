import cv2
import serial
import numpy as np
import time
from ultralytics import YOLO

# --- CẤU HÌNH ---
COM_PORT = 'COM8'
BAUD_RATE = 921600

print(f"Đang kết nối tới {COM_PORT}...")
try:
    ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=2.0)
    print("Kết nối Serial thành công!")
except Exception as e:
    print(f"Lỗi kết nối Serial: {e}")
    exit()

# Khởi tạo model YOLOv8 (Tự động tải file yolov8n.pt nếu chưa có)
print("Đang tải model YOLOv8n...")
model = YOLO('yolov8n-pose.pt')
print("Model đã sẵn sàng!")


def get_frame():
    # Giữ nguyên hàm get_frame() chuẩn xác của bạn ở bước trước
    ser.reset_input_buffer()
    ser.write(b"REQ_FRAME\n")
    ser.flush()

    try:
        header_bytes = ser.readline()
        if not header_bytes: return None
        header = header_bytes.decode('utf-8').strip()
    except:
        return None

    if header.startswith("[PYTHON_IMG:"):
        try:
            expected_size = int(header.replace("[PYTHON_IMG:", "").replace("]", ""))
            if expected_size > 0:
                image_data = ser.read(expected_size)
                if len(image_data) == expected_size:
                    np_arr = np.frombuffer(image_data, np.uint8)
                    return cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
        except ValueError:
            pass
    return None




# --- VÒNG LẶP CHÍNH ---
prev_time = time.time()
frame_count = 0
fps = 0

while True:
    frame = get_frame()
    if frame is not None:
        # 1. Phóng to ảnh để dễ nhìn (Tùy chọn)
        frame = cv2.resize(frame, None, fx=2.0, fy=2.0, interpolation=cv2.INTER_LINEAR)
        height, width, _ = frame.shape

        left_bound = int(width * 0.35)

        right_bound = int(width * 0.65)

        # 2. Đưa khung hình vào AI (chỉ tìm class 0: person)
        # results = model.predict(source=frame, classes=[0], conf=0.5, verbose=False)
        results = model.predict(source=frame, conf=0.5, verbose=False)

        label = 3  # Mặc định 3: TRONG (Không có người)
        # max_area = 0
        # target_box = None
        target_nose = None

        for result in results:
            # Kiểm tra xem có nhận diện được người và có tọa độ keypoints không
            if result.keypoints is not None and len(result.keypoints.xy) > 0:
                # Lấy mảng 17 điểm của người đầu tiên phát hiện được
                keypoints = result.keypoints.xy[0].cpu().numpy()

                # Điểm số 0 chính là cái Mũi (Nose)
                nose_x, nose_y = int(keypoints[0][0]), int(keypoints[0][1])

                # YOLO sẽ trả về (0,0) nếu điểm đó bị che khuất / không thấy
                if nose_x > 0 and nose_y > 0:
                    target_nose = (nose_x, nose_y)

                    # QUYẾT ĐỊNH VÙNG DỰA TRÊN TỌA ĐỘ CỦA CÁI MŨI

                    if nose_x < left_bound:
                        label = 0
                    elif nose_x < right_bound:
                        label = 1
                    else:
                        label = 2

                    break  # Chỉ lấy điểm mũi của 1 người để quạt bám theo

        # 4. Gửi kết quả xuống ESP32-S3
        cmd = f"AI:{label}\n"
        ser.write(cmd.encode('utf-8'))

        # --- PHẦN VẼ LÊN MÀN HÌNH ĐỂ DEBUG ---
        # Vẽ các đường chia 3 vùng
        cv2.line(frame, (left_bound, 0), (left_bound, height), (255, 255, 255), 1)
        cv2.line(frame, (right_bound, 0), (right_bound, height), (255, 255, 255), 1)


        if target_nose:
            # Vẽ một chấm tròn to màu đỏ ngay mũi bạn
            cv2.circle(frame, target_nose, 8, (0, 0, 255), -1)

        # Hiển thị text trạng thái
        zone_texts = {0: "TRAI", 1: "GIUA", 2: "PHAI", 3: "TRONG"}
        cv2.putText(frame, f"Trang thai: {zone_texts[label]}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX,
                    0.8, (0, 255, 255),2)

        # Tính và hiển thị FPS
        frame_count += 1
        if time.time() - prev_time >= 1.0:
            fps = frame_count
            frame_count = 0
            prev_time = time.time()
        cv2.putText(frame, f"FPS: {fps}", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.8,
                    (0, 255, 0), 2)

        cv2.imshow("Smart Fan - AI Tracking", frame)

    else:
        # NẾU LỖI: In ra màn hình và chờ 0.5 giây để tránh spam
        print("[LỖI] Không thể giải mã khung hình hoặc ESP32-S3 không gửi!")
        time.sleep(0.5)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

ser.close()
cv2.destroyAllWindows()