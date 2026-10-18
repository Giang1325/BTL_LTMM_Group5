# Lý thuyết & Bối cảnh: Mã dòng và Dự án eSTREAM

## 1. Mã dòng đồng bộ và So sánh với mã khối
**Mã dòng đồng bộ (Synchronous Stream Cipher):**
Là loại thuật toán mã hóa mà chuỗi khóa giả ngẫu nhiên (keystream) được sinh ra độc lập với bản rõ (plaintext) và bản mã (ciphertext). Quá trình mã hóa và giải mã diễn ra bằng cách XOR từng bit (hoặc byte) của bản rõ với từng bit của keystream. Yêu cầu bắt buộc là bộ sinh keystream ở phía gửi và phía nhận phải được đồng bộ về mặt trạng thái và thời gian.

**So sánh với mã khối (Block Cipher):**
- **Đơn vị xử lý và Đệm (Padding):** Mã dòng xử lý từng bit/byte liên tục nên không cần đệm. Mã khối chia dữ liệu thành các khối cố định (ví dụ 128 bit đối với AES). Việc có cần dùng đệm hay không phụ thuộc vào chế độ hoạt động (Mode of Operation); các chế độ như ECB hoặc CBC yêu cầu đệm, trong khi chế độ CTR hoặc OFB cho phép mã khối hoạt động như mã dòng và không cần đệm.
- **Hiệu năng & Tài nguyên:** Nhìn chung, các thuật toán mã dòng thiết kế cho phần cứng (như Trivium) thường tiêu thụ ít diện tích cổng logic hơn. Tuy nhiên, việc so sánh mã khối hay mã dòng tốn tài nguyên hơn không hoàn toàn tuyệt đối mà phụ thuộc vào thuật toán cụ thể và cách tối ưu hóa khi cài đặt trên thực tế.
- **Lan truyền lỗi:** Đối với mã dòng đồng bộ, nếu 1 bit trên đường truyền bị lật, chỉ có 1 bit bản rõ tương ứng bị sai. Đối với mã khối, tính chất lan truyền lỗi phụ thuộc vào chế độ hoạt động (ví dụ: chế độ CBC sẽ làm hỏng toàn bộ khối hiện tại và 1 bit của khối tiếp theo, trong khi chế độ CTR chỉ sai đúng 1 bit).

**Vì sao không được dùng lại cặp Khóa (Key) và Vector khởi tạo (IV):**
Trong mã dòng, thuật toán sinh keystream phụ thuộc hoàn toàn vào cặp Key/IV. Nếu sử dụng lại một cặp Key/IV cho hai thông điệp khác nhau ($P_1$ và $P_2$), hệ thống sẽ sinh ra cùng một chuỗi keystream ($K$).
Khi đó: 
$C_1 = P_1 \oplus K$
$C_2 = P_2 \oplus K$
Kẻ tấn công chỉ cần XOR hai bản mã với nhau: $C_1 \oplus C_2 = P_1 \oplus P_2$. 
Keystream bị triệt tiêu, kẻ tấn công có được hàm XOR của hai bản rõ, từ đó dễ dàng dùng các kỹ thuật phân tích thống kê để khôi phục lại hai thông điệp ban đầu (tấn công Two-time pad).

## 2. Dự án eSTREAM và Vị trí của Trivium
**Dự án eSTREAM:**
Được tổ chức bởi mạng lưới ECRYPT của Châu Âu (2004 - 2008), eSTREAM có mục tiêu tìm kiếm và đánh giá các thuật toán mã dòng mới, đáp ứng các nhu cầu mà chuẩn AES chưa bao phủ tốt, đặc biệt là tốc độ xử lý phần mềm cao hoặc tính tinh gọn trên phần cứng.

**Hai Profile của eSTREAM:**
- **Profile 1 (Phần mềm):** Dành cho mã dòng có tốc độ thực thi phần mềm vượt trội hơn AES-128 trong môi trường máy tính thông thường.
- **Profile 2 (Phần cứng):** Dành cho mã dòng thiết kế tối ưu cho phần cứng bị hạn chế tài nguyên (số lượng cổng logic thấp, điện năng tiêu thụ ít, bộ nhớ nhỏ) như thẻ RFID, mạng cảm biến, vi điều khiển.

**Danh mục cuối cùng (Final Portfolio 2008):**
- *Profile 1:* HC-128, Rabbit, Salsa20/12, SOSEMANUK.
- *Profile 2:* Grain v1, MICKEY 2.0, Trivium. (Lưu ý: Thuật toán F-FCSR-H v2 từng nằm trong danh mục Profile 2 được công bố vào tháng 4/2008, nhưng đã bị loại bỏ ở bản sửa đổi vào tháng 9/2008 do phát hiện lỗ hổng bảo mật).

**Vị trí của Trivium:**
Trivium được chọn vào danh mục cuối cùng của eSTREAM thuộc Profile 2. Thuật toán này không sử dụng các hộp thế (S-box) mà sử dụng 3 thanh ghi dịch hồi tiếp phi tuyến (NFSR), mang lại hiệu năng sinh keystream tốt trên diện tích silicon nhỏ.

## 3. Trivium trong chuẩn ISO/IEC 29192-3
Dựa trên kết quả từ dự án eSTREAM, Trivium đã được tổ chức ISO đưa vào chuẩn **ISO/IEC 29192-3:2012**. Đây là bộ tiêu chuẩn quốc tế chuyên biệt dành cho "Mật mã hạng nhẹ" (Lightweight Cryptography), trong đó Phần 3 chuyên chuẩn hóa các thuật toán mã hóa dòng.

---
## Tài liệu tham khảo
1. The eSTREAM Project (ECRYPT Stream Cipher Project).
2. Christophe De Cannière, Bart Preneel, *Trivium Specifications*.
3. ISO/IEC 29192-3:2012, *Information technology — Security techniques — Lightweight cryptography — Part 3: Stream ciphers*.
