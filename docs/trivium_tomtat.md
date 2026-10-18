# TÓM TẮT ĐẶC TẢ THUẬT TOÁN TRIVIUM
*Người thực hiện: Hoàng Gia Bảo*

## 1. Tổng quan và Cấu trúc 3 thanh ghi (93 / 84 / 111 bit)

Trạng thái nội tại của Trivium có tổng kích thước **288 bit**, bao gồm 3 thanh ghi dịch có độ dài khác nhau:

- **Thanh ghi A ($a$):** Độ dài **93 bit**.
- **Thanh ghi B ($b$):** Độ dài **84 bit**.
- **Thanh ghi C ($c$):** Độ dài **111 bit**.

Ở mỗi vòng, 1 bit mới sẽ được tính toán và dịch vào đầu mỗi thanh ghi dựa trên sự kết hợp phi tuyến của các điểm lấy mẫu từ chính thanh ghi đó và một thanh ghi khác; đồng thời thuật toán xuất ra 1 bit đầu ra (keystream).

---

## 2. Đặc tả toán học: Các bit dùng mỗi vòng và Sinh Keystream

Thuật toán Trivium được biểu diễn ngắn gọn qua 3 phương trình đệ quy trên trường hữu hạn $\text{GF}(2)$. Trong đó, mỗi biến đại diện cho 1 bit, phép cộng ($+$) là phép **XOR** và phép nhân ($\cdot$) là phép **AND**.

### 2.1. Ba phương trình đệ quy cập nhật trạng thái (Các bit được dùng mỗi vòng)

Tại vòng thứ $i$, bit mới đi vào 3 thanh ghi A, B, C lần lượt là $a_i, b_i, c_i$ được tính như sau:

- $a_i = c_{i-66} + c_{i-111} + c_{i-110} \cdot c_{i-109} + a_{i-69}$
- $b_i = a_{i-66} + a_{i-93} + a_{i-92} \cdot a_{i-91} + b_{i-78}$
- $c_i = b_{i-69} + b_{i-84} + b_{i-83} \cdot b_{i-82} + c_{i-87}$

**Giải thích các vị trí bit lấy mẫu ở mỗi vòng:**

- **Để cập nhật thanh ghi A ($a_i$):** Lấy mẫu tại các vị trí cách đó 66, 111, 110, 109 bước của thanh ghi C (trong đó bit 110 và 109 được AND với nhau để tạo tính phi tuyến) kết hợp (XOR) với bit cách đó 69 bước của thanh ghi A.
- **Để cập nhật thanh ghi B ($b_i$):** Lấy mẫu tại các vị trí cách đó 66, 93, 92, 91 bước của thanh ghi A (bit 92 và 91 AND với nhau) kết hợp (XOR) với bit cách đó 78 bước của thanh ghi B.
- **Để cập nhật thanh ghi C ($c_i$):** Lấy mẫu tại các vị trí cách đó 69, 84, 83, 82 bước của thanh ghi B (bit 83 và 82 AND với nhau) kết hợp (XOR) với bit cách đó 87 bước của thanh ghi C.

### 2.2. Phương trình sinh bit đầu ra (Keystream $r_i$)

Các bit luồng khóa đầu ra $r_0 \dots r_{2^{64}-1}$ (dùng để XOR với bản rõ) được tạo ra bằng cách cộng (XOR) 6 vị trí lấy mẫu từ cả 3 thanh ghi:

- $r_i = c_{i-66} + c_{i-111} + a_{i-66} + a_{i-93} + b_{i-69} + b_{i-84}$

### 2.3. Quy ước ghép chuỗi bit thành chuỗi byte ($R_i$)

Để ánh xạ từ dòng bit đầu ra $r$ sang dòng byte $R$, Trivium sử dụng quy ước **LSb-first** (bit có trọng số nhỏ nhất xếp trước) theo công thức:

- $R_i = \sum_{j=0,\dots,7} 2^j r_{8i+j}$

---

## 3. Giai đoạn nạp Khóa / IV và 1152 vòng khởi tạo

### 3.1. Nạp Khóa (Key) và Vector khởi tạo (IV)

Với đầu vào là khóa $K$ dài 80-bit ($k_0 \dots k_{79}$) và vector khởi tạo $IV$ dài $l$-bit ($v_0 \dots v_{l-1}$, với $0 \le l \le 80$), trạng thái ban đầu của 3 thanh ghi được thiết lập như sau:

- **Thanh ghi A (93 bit):** $(a_{-1245} \dots a_{-1153}) = (0, 0 \dots 0, k_0 \dots k_{79})$
  *(13 bit đầu được điền `0`, 80 bit tiếp theo nạp khóa $k_0 \dots k_{79}$)*.

- **Thanh ghi B (84 bit):** $(b_{-1236} \dots b_{-1153}) = (0, 0 \dots 0, v_0 \dots v_{l-1})$
  *(Các bit đầu được điền `0`, $l$ bit cuối nạp $v_0 \dots v_{l-1}$)*.

- **Thanh ghi C (111 bit):** $(c_{-1263} \dots c_{-1153}) = (1, 1, 1, 0, 0 \dots 0)$
  *(3 bit đầu tiên $c_{-1263}, c_{-1262}, c_{-1261}$ được cố định bằng `1`, 108 bit còn lại điền `0`)*.

### 3.2. Ý nghĩa của 1152 vòng khởi tạo

- Các chỉ số âm lớn trên các giá trị khởi tạo (từ $-1263$ đến $-1153$) thể hiện rằng thuật toán bắt buộc phải chạy qua **$4 \times 288 = 1152$ bước cập nhật** (tương ứng với chỉ số $i$ chạy từ $-1152$ đến $-1$) trước khi bit đầu ra đầu tiên ($r_0$ tại $i = 0$) được sinh ra.
- Trong suốt 1152 vòng khởi tạo này, trạng thái 3 thanh ghi được cập nhật liên tục theo 3 phương trình đệ quy nhưng **không xuất ra bất kỳ bit đầu ra nào**.
- **Mục đích:** Đảm bảo sau 1152 vòng, mọi bit trong trạng thái nội tại 288-bit đều phụ thuộc vào mọi bit của Key và IV theo một quan hệ phi tuyến phức tạp, giúp chống lại các tấn công thám mã.

---

## 4. Tính chất 64 vòng và Cơ sở cho bản tối ưu 64-bit

- **Tính chất không lấy mẫu ở 65 bit đầu:** Quan sát các chỉ số trễ trong hệ phương trình đệ quy ($i-66, i-69, i-78, i-84, \dots$), điểm lấy mẫu gần nhất cách vị trí hiện tại 66 bước ($i-66$). Do đó, **không có bất kỳ điểm lấy mẫu (tap) nào xuất hiện trên 65 bit đầu tiên của mỗi thanh ghi dịch**.
- **Hệ quả:** Mỗi bit trạng thái mới được sinh ra ($a_i, b_i, c_i$) sẽ không được dùng làm đầu vào tính toán trong ít nhất **65 vòng tiếp theo**.
- **Ứng dụng tối ưu hóa hiệu năng (64-bit):**
  - *Trên phần cứng:* Thay vì tạo 1 bit/chu kỳ với 3488 cổng logic, có thể tính song song 64 bit trạng thái cùng lúc với chi phí 5504 cổng logic.
  - *Trên phần mềm (`trivium64.c`):* Tính chất này cho phép cài đặt thuật toán theo kỹ thuật **bitslice** (tính toán song song 64 vòng cùng lúc trên các biến nguyên 64-bit `uint64_t` bằng phép toán bitwise XOR và AND). Nhờ đó, tốc độ mã hóa đạt khoảng **4 cycles/byte** trên nền tảng x86, vượt trội hơn nhiều so với mức 19 cycles/byte của bản cài đặt AES tham chiếu trên cùng hệ thống.