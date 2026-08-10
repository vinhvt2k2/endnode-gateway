Gateway_Modular - Tach tu file Gateway.ino

Cac module:
- GatewayConfig.h      : cau hinh WiFi, Firebase, LoRa, OLED, AES, validate, history.
- DataTypes.h          : struct NodeInfo va SensorData.
- Globals.h/.cpp       : bien global Firebase, OLED, danh sach node, queue.
- NetworkManager       : ket noi WiFi va Firebase.
- OledManager          : khoi tao va hien thi trang thai OLED.
- LoraManager          : khoi tao LoRa, nhan packet, xu ly CONFIG/VALUE, gui ACK.
- SecurityManager      : AES-128-CCM, nonce, AAD, Base64, build packet {m,c,d,t}.
- ValidationManager    : kiem tra du lieu cam bien hop le.
- QueueManager         : dua du lieu hop le vao queue.
- FirebaseManager      : gui current/history len Firebase, giu 12 ban ghi gan nhat.
- DemoPrinter          : cac log Serial phuc vu demo.

