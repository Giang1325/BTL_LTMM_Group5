# Giải thích Chương trình Mã hóa và Tiện ích Hex (`hex.c`, `main.c`)

## 1. Tổng quan vai trò trong hệ thống và Bối cảnh lý thuyết

### 1.1. Bối cảnh lý thuyết: Mã dòng và Dự án eSTREAM
- **Mã dòng đồng bộ (Synchronous Stream Cipher):** Là thuật toán mật mã sinh ra chuỗi khóa giả ngẫu nhiên (keystream) độc lập với bản rõ và bản mã dựa trên Khóa (Key) và Vector khởi tạo (IV). Quá trình mã hóa và giải mã thực hiện phép XOR từng bit/byte giữa dữ liệu và keystream.
- **Dự án eSTREAM và chuẩn ISO/IEC 29192-3:** Dự án eSTREAM (2004–2008) của mạng lưới ECRYPT Châu Âu nhằm tìm kiếm các mã dòng hiện đại theo 2 hồ sơ: Profile 1 (Phần mềm tốc độ cao: HC-128, Rabbit, Salsa20/12, SOSEMANUK) và Profile 2 (Phần cứng tài nguyên hạn chế: Grain v1, MICKEY 2.0, Trivium). Trivium được chọn ở Profile 2 nhờ cấu trúc 3 thanh ghi dịch hồi tiếp phi tuyến (NFSR) gọn nhẹ (288 bit trạng thái) và sau đó được chuẩn hóa quốc tế trong **ISO/IEC 29192-3:2012** (Mật mã hạng nhẹ).

### 1.2. Vị trí của module trong hệ thống
Phần việc này bao gồm 3 file (`src/hex.h`, `src/hex.c`, `src/main.c`), đóng vai trò là **giao diện người dùng (CLI) và tầng xử lý dữ liệu vào/ra** của toàn bộ dự án:
- **Gọi hàm của ai:** `src/main.c` gọi trực tiếp các hàm lõi trong `src/trivium.h` của **Giang** (`trivium_init`, `trivium_keystream`, `trivium_crypt`) để khởi tạo trạng thái và mã hóa dữ liệu.
- **Ai gọi hàm của mình:** `src/main.c` sử dụng các hàm chuyển đổi trong `src/hex.h` (`hex_to_bytes`, `print_hex`). Ngoài ra, các công cụ kiểm thử của **Hào** và **Bảo** cũng có thể dùng `hex.c` khi cần xử lý chuỗi Hex.

---

## 2. Cách chạy chương trình và Kết quả mẫu

Chương trình biên dịch qua lệnh `make app` và cung cấp 3 chế độ hoạt động:

### 2.1. Lệnh sinh Keystream (`ks`)
Dùng để kiểm tra nhanh chuỗi keystream sinh ra từ một cặp Khóa và IV (dạng Hex 20 ký tự = 10 byte), đối chiếu trực tiếp với Test Vector của eSTREAM.
```bash
$ ./trivium ks 00000000000000000000 00000000000000000000 16
FBE0BF265859051B517A2E4E239FC97F
