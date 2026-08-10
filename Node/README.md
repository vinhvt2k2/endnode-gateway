# Modular Node AES-CCM Project

Gói này tách 2 file Node1.ino và Node2.ino thành các module `.h/.cpp`.

## Thư mục

- `Node1_Modular/`: code cho NODE_01
- `Node2_Modular/`: code cho NODE_02

## Các module chính

- `NodeConfig.h`: cấu hình chân, LoRa, sleep, bảo mật, ngưỡng validation, struct `SensorData`.
- `Globals.h` / `Globals.cpp`: khai báo/khởi tạo biến toàn cục, cảm biến, bộ lọc, key AES, MAC node, biến RTC.
- `NodeState.h` / `NodeState.cpp`: lưu/đọc `configConfirmed`, `securityCounter` bằng `Preferences`.
- `Utils.h` / `Utils.cpp`: hàm tiện ích, kiểm tra range, in log Serial.
- `SensorManager.h` / `SensorManager.cpp`: đọc cảm biến, lọc dữ liệu, tính AQI/PreAQI, tạo payload VALUE.
- `SecurityManager.h` / `SecurityManager.cpp`: AES-128-CCM, Base64, nonce, AAD, build packet `{m,c,d,t}`.
- `LoraManager.h` / `LoraManager.cpp`: khởi tạo LoRa, gửi packet, chờ ACK mã hóa, CONFIG handshake, deep sleep.
- `Node1_Modular.ino` / `Node2_Modular.ino`: chỉ còn `setup()` và `loop()`.

## Lưu ý

- Node 1 dùng MAC `NODE_01` và key tương ứng trong `Node1_Modular/Globals.cpp`.
- Node 2 dùng MAC `NODE_02` và key tương ứng trong `Node2_Modular/Globals.cpp`.
- Nếu muốn đổi MAC/key, sửa trong `Globals.cpp` của node tương ứng.
- Giữ nguyên các thư viện ngoài như `KalmanFilter.h`, `VN_AQI.h`, `linear_regression_model.h`, `SdsDustSensor.h` trong môi trường Arduino của bạn.
