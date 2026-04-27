// #include "tinyML.h"

// // 1. KHAI BÁO BIẾN TOÀN CỤC CHO TINYML

// const tflite::Model *model = nullptr;
// tflite::MicroInterpreter *interpreter = nullptr;
// TfLiteTensor *input = nullptr;
// TfLiteTensor *output = nullptr;

// // Tensor Arena: Không gian RAM để mô hình nháp tính toán
// // Mô hình MobileNetV2 lượng tử hóa thường cần khoảng 800KB - 1MB
// const int tensor_arena_size = 130 * 1024;
// uint8_t *tensor_arena = nullptr;
// static tflite::MicroErrorReporter micro_error_reporter;
// tflite::ErrorReporter *error_reporter = &micro_error_reporter;

// // Tên các nhãn (Phải khớp thứ tự với class_names trong Python)
// const char *LABELS[] = {"LEFT", "CENTER", "RIGHT", "BACKGROUND"};

// void init_tinyml()
// {
//     tflite::InitializeTarget();

//     // Đọc mô hình từ mảng byte trong file .h
//     model = tflite::GetModel(model_tflite);
//     if (model->version() != TFLITE_SCHEMA_VERSION)
//     {
//         Serial.printf("Lỗi: Phiên bản Schema mô hình (%d) không khớp với TFLM (%d)\n",
//                       model->version(), TFLITE_SCHEMA_VERSION);
//         return;
//     }

//     // CẤP PHÁT PSRAM CỰC KỲ QUAN TRỌNG
//     // Nếu dùng malloc() bình thường, ESP32 sẽ sập ngay lập tức vì tràn SRAM
//     // tensor_arena = (uint8_t *)heap_caps_malloc(tensor_arena_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

//     tensor_arena = (uint8_t *)heap_caps_aligned_alloc(16, tensor_arena_size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
//     if (tensor_arena == nullptr)
//     {
//         Serial.println("Lỗi: Không đủ PSRAM để cấp phát Tensor Arena!");
//         return;
//     }

//     // Load các phép toán (Operations) hỗ trợ
//     static tflite::AllOpsResolver resolver;

//     // Khởi tạo Bộ suy luận (Interpreter)
//     static tflite::MicroInterpreter static_interpreter(
//         model, resolver, tensor_arena, tensor_arena_size, error_reporter);
//     interpreter = &static_interpreter;

//     // Cấp phát bộ nhớ cho các biến tính toán bên trong
//     TfLiteStatus allocate_status = interpreter->AllocateTensors();

//     Serial.printf("Kích thước Arena đã sử dụng: %d bytes\n", interpreter->arena_used_bytes());
//     if (allocate_status != kTfLiteOk)
//     {
//         Serial.println("Lỗi: AllocateTensors() thất bại.");
//         return;
//     }

//     // Trỏ con trỏ vào đầu vào và đầu ra của mô hình
//     input = interpreter->input(0);
//     output = interpreter->output(0);

//     Serial.printf("Khởi tạo TinyML thành công!\n");
//     Serial.printf("Kích thước đầu vào cần thiết: %d x %d x %d\n",
//                   input->dims->data[1], input->dims->data[2], input->dims->data[3]);
// }

// // ==========================================
// // 3. HÀM CHẠY SUY LUẬN (Gọi trong loop)
// // ==========================================
// int run_tinyml()
// {
//     // Lưu ý: Trước khi gọi hàm này, bạn phải nạp dữ liệu ảnh (đã resize về 96x96,
//     // chuyển đổi hệ màu và chuẩn hóa về kiểu int8) vào vùng nhớ: input->data.int8

//     if (interpreter == nullptr || output == nullptr)
//     {
//         return -1;
//     }

//     unsigned long start_time = millis();

//     // Bắt đầu chạy mạng nơ-ron (Invoke)
//     TfLiteStatus invoke_status = interpreter->Invoke();
//     if (invoke_status != kTfLiteOk)
//     {
//         Serial.println("Lỗi: Invoke() thất bại.");
//         return -1;
//     }

//     unsigned long end_time = millis();
//     Serial.printf("Thời gian suy luận: %lu ms\n", end_time - start_time);

//     // Tìm nhãn có xác suất cao nhất (Argmax)
//     int max_index = 0;
//     int8_t max_val = output->data.int8[0];

//     for (int i = 1; i < 4; i++)
//     {
//         if (output->data.int8[i] > max_val)
//         {
//             max_val = output->data.int8[i];
//             max_index = i;
//         }
//     }

//     // Xác suất (confidence) được lưu dưới dạng số nguyên int8 (từ -128 đến 127)
//     // Bạn có thể map nó về 0-100% nếu muốn hiển thị
//     Serial.printf("Dự đoán: %s (Index: %d, Raw Value: %d)\n", LABELS[max_index], max_index, max_val);

//     return max_index; // Trả về 0(LEFT), 1(CENTER), 2(RIGHT), 3(BG)
// }

// bool process_image_to_tensor(camera_fb_t *fb)
// {
//     if (input == nullptr)
//     {
//         Serial.println("Lỗi: Mô hình chưa sẵn sàng. Bỏ qua frame này.");
//         return false;
//     }

// #ifdef GRAYMODE
//     const int target_w = 96;
//     const int target_h = 96;

//     // Tính tỷ lệ co giãn giữa ảnh Camera (320x240) và ảnh Tensor (96x96)
//     float scale_x = (float)fb->width / target_w;
//     float scale_y = (float)fb->height / target_h;

//     int tensor_idx = 0; // Biến chạy cho mảng Tensor 1 chiều

//     for (int y = 0; y < target_h; y++)
//     {
//         for (int x = 0; x < target_w; x++)
//         {
//             // Ánh xạ tọa độ
//             int src_x = (int)(x * scale_x);
//             int src_y = (int)(y * scale_y);

//             // Vì là ảnh Grayscale (1 kênh), công thức tính Index vô cùng đơn giản
//             int src_idx = src_y * fb->width + src_x;

//             // Lấy giá trị độ sáng (0 - 255) trực tiếp từ buffer camera
//             uint8_t pixel_val = fb->buf[src_idx];

//             // BƯỚC 3: LƯỢNG TỬ HÓA VÀ NẠP VÀO TENSOR (1 kênh màu)
//             // Ép dải giá trị [0, 255] về dải [-128, 127]
//             input->data.int8[tensor_idx++] = (int8_t)pixel_val - 128;
//         }
//     }

//     return true;

// #else

//     // Kích thước chuẩn của mô hình
//     const int target_w = 96;
//     const int target_h = 96;

//     // BƯỚC 1: GIẢI MÃ JPEG SANG MẢNG RGB888
//     // Tính toán kích thước bộ nhớ cần thiết: Width * Height * 3 byte (R, G, B)
//     size_t rgb_size = fb->width * fb->height * 3;

//     // Ép ESP32 lấy RAM từ cục PSRAM để giải mã (Nếu dùng RAM trong sẽ tràn ngay)
//     uint8_t *rgb_buf = (uint8_t *)heap_caps_malloc(rgb_size, MALLOC_CAP_SPIRAM);
//     if (!rgb_buf)
//     {
//         Serial.println("Lỗi: Không đủ PSRAM để giải mã ảnh JPEG!");
//         return false;
//     }

//     // Hàm có sẵn của ESP32 để chuyển đổi định dạng
//     bool converted = fmt2rgb888(fb->buf, fb->len, fb->format, rgb_buf);
//     if (!converted)
//     {
//         Serial.println("Lỗi: Giải mã JPEG sang RGB888 thất bại.");
//         free(rgb_buf);
//         return false;
//     }

//     // BƯỚC 2: THU NHỎ ẢNH (RESIZE) BẰNG NEAREST NEIGHBOR & LƯỢNG TỬ HÓA
//     // Tính tỷ lệ co giãn giữa ảnh gốc (VD: 160x120) và ảnh đích (96x96)
//     float scale_x = (float)fb->width / target_w;
//     float scale_y = (float)fb->height / target_h;

//     int tensor_idx = 0; // Biến chạy cho mảng Tensor 1 chiều

//     for (int y = 0; y < target_h; y++)
//     {
//         for (int x = 0; x < target_w; x++)
//         {
//             // Ánh xạ tọa độ (x, y) của ảnh 96x96 ngược về tọa độ của ảnh gốc
//             int src_x = (int)(x * scale_x);
//             int src_y = (int)(y * scale_y);

//             // Tính vị trí pixel trong mảng 1D RGB888
//             int src_idx = (src_y * fb->width + src_x) * 3;

//             // Lấy giá trị màu nguyên thủy (từ 0 đến 255)
//             uint8_t r = rgb_buf[src_idx];
//             uint8_t g = rgb_buf[src_idx + 1];
//             uint8_t b = rgb_buf[src_idx + 2];

//             // BƯỚC 3: LƯỢNG TỬ HÓA VÀ NẠP VÀO TENSOR
//             // Ép dải giá trị [0, 255] về dải [-128, 127] bằng cách trừ đi 128
//             input->data.int8[tensor_idx++] = (int8_t)r - 128;
//             input->data.int8[tensor_idx++] = (int8_t)g - 128;
//             input->data.int8[tensor_idx++] = (int8_t)b - 128;
//         }
//     }

//     // Dọn rác bộ nhớ ngay lập tức để tránh Memory Leak
//     free(rgb_buf);
//     return true;
// #endif
// }
