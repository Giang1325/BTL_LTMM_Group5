# Lý thuyết & Bối cảnh: Mã dòng và Dự án eSTREAM

## 1. Mã dòng đồng bộ và So sánh với mã khối
**Mã dòng đồng bộ (Synchronous Stream Cipher):**
Là loại thuật toán mã hóa mà chuỗi khóa giả ngẫu nhiên (keystream) được sinh ra một cách độc lập với bản rõ (plaintext) và bản mã (ciphertext). Quá trình mã hóa và giải mã diễn ra bằng cách XOR từng bit (hoặc byte) của bản rõ với từng bit của keystream. Yêu cầu bắt buộc là bộ sinh keystream ở phía gửi và phía nhận phải được đồng bộ hoàn hảo về mặt trạng thái và thời gian.

**So sánh ngắn với mã khối (Block Cipher):**
- **Đơn vị xử lý:** Mã dòng xử lý từng bit/byte một cách liên tục; mã khối chia dữ liệu thành các khối cố định (ví dụ 128 bit đối với AES) và phải dùng thêm đệm (padding) nếu thiếu.
- **Hiệu năng & Tài nguyên:** Mã dòng thường có tốc độ thực thi nhanh hơn và tốn rất ít tài nguyên phần cứng (số lượng cổng logic thấp), lý tưởng cho thiết bị nhúng và luồng dữ liệu thời gian thực. Mã khối tốn nhiều bộ nhớ và năng lượng hơn.
- **Lan truyền lỗi:** Ở mã dòng đồng bộ, nếu 1 bit trên đường truyền bị lật, chỉ có 1 bit bản rõ tương ứng bị sai. Ở mã khối, sai 1 bit trên đường truyền có thể làm hỏng toàn bộ khối dữ liệu giải mã.

**Vì sao tuyệt đối không được dùng lại cặp Khóa (Key) và Vector khởi tạo (IV):**
Trong mã dòng, thuật toán sinh keystream phụ thuộc hoàn toàn vào cặp Key/IV. Nếu sử dụng lại một cặp Key/IV cho hai thông điệp khác nhau ($P_1$ và $P_2$), hệ thống sẽ sinh ra cùng một chuỗi keystream ($K$).
Khi đó: 
$C_1 = P_1 \oplus K$
$C_2 = P_2 \oplus K$
Kẻ tấn công chỉ cần XOR hai bản mã với nhau: $C_1 \oplus C_2 = P_1 \oplus P_2$. 
Keystream bị triệt tiêu hoàn toàn, kẻ tấn công có được hàm XOR của hai bản rõ, từ đó rất dễ dàng dùng các kỹ thuật phân tích thống kê ngôn ngữ để khôi phục lại cả hai thông điệp ban đầu (tấn công Two-time pad).

## 2. Dự án eSTREAM và Vị trí của Trivium
**Dự án eSTREAM:**
Được tổ chức bởi mạng lưới ECRYPT của Châu Âu (diễn ra từ 2004 đến 2008), eSTREAM có mục tiêu tìm kiếm và đánh giá các thuật toán mã dòng mới có tính bảo mật cao, nhằm đáp ứng các nhu cầu mà chuẩn AES (mã khối) chưa bao phủ tốt, đặc biệt là về tốc độ cực cao hoặc tính tinh gọn trên phần cứng cực nhỏ.

**Hai Profile (Hồ sơ) của eSTREAM:**
- **Profile 1 (Phần mềm - Software):** Dành cho các thuật toán mã dòng có tốc độ thực thi phần mềm vượt trội hơn AES-128 trong các môi trường máy tính thông thường với tài nguyên dồi dào.
- **Profile 2 (Phần cứng - Hardware):** Dành cho các thuật toán mã dòng thiết kế tối ưu cho phần cứng bị hạn chế nghiêm trọng về tài nguyên (với số lượng cổng logic cực thấp, tiêu thụ ít điện năng, bộ nhớ nhỏ) như thẻ RFID, mạng cảm biến không dây, vi điều khiển nhỏ.

**Danh mục cuối cùng (Final Portfolio 2008):**
Kết thúc dự án, danh mục được chọn gồm:
- *Profile 1:* HC-128, Rabbit, Salsa20, SOSEMANUK.
- *Profile 2:* Grain v1, MICKEY 2.0, Trivium.

**Vị trí của Trivium:**
Trivium xuất sắc lọt vào danh mục cuối cùng của eSTREAM thuộc Profile 2 (Phần cứng). Thuật toán này nổi bật nhờ thiết kế vô cùng thanh lịch và gọn gàng, hoàn toàn không sử dụng các hộp thế (S-box) phức tạp mà chỉ dùng 3 thanh ghi dịch hồi tiếp phi tuyến (NFSR). Nhờ đó, Trivium mang lại hiệu năng sinh keystream rất tốt trên diện tích silicon cực kỳ nhỏ.

## 3. Trivium trong chuẩn ISO/IEC 29192-3
Dựa trên sự thành công và mức độ bảo mật đã được cộng đồng quốc tế thẩm định qua dự án eSTREAM, Trivium tiếp tục được công nhận rộng rãi trong các tiêu chuẩn công nghiệp. Cụ thể, Trivium đã được tổ chức ISO (Tổ chức Tiêu chuẩn hóa Quốc tế) chính thức đưa vào chuẩn **ISO/IEC 29192-3**.
Bộ tiêu chuẩn ISO/IEC 29192 là bộ tiêu chuẩn quốc tế chuyên biệt dành cho "Mật mã hạng nhẹ" (Lightweight Cryptography). Trong đó, Phần 3 (Part 3) chuyên chuẩn hóa các thuật toán mã hóa dòng. Sự kiện này khẳng định vị thế của Trivium như một giải pháp bảo mật tiêu chuẩn quốc tế, tin cậy cho các ứng dụng IoT và hệ thống nhúng giới hạn tài nguyên.
