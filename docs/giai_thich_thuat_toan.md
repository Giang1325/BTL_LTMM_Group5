# TÀI LIỆU NỀN TẢNG: GIẢI THÍCH THUẬT TOÁN TRIVIUM VÀ KẾT QUẢ KIỂM THỬ
**Người thực hiện:** Hoàng Gia Bảo - 20233276

**Thành phần:** `tools/trace.c`, `tools/avalanche.c`, `docs/giai_thich_thuat_toan.md`

Đây là tài liệu đặc tả kỹ thuật của dự án. Mọi thành viên cần đọc tài liệu này đầu tiên để nắm vững luồng hoạt động của thuật toán lõi, cách đọc kết quả kiểm thử hộp trắng, và minh chứng toán học cho độ an toàn của hệ mã hóa Trivium.

---

## 1. Tổng quan thuật toán Trivium
Trivium là một hệ mật mã dòng được thiết kế tối ưu cho phần cứng, hoạt động dựa trên một trạng thái nội bộ dài **288 bit**. Trạng thái này được chia làm 3 thanh ghi dịch liên kết vòng với nhau:
*   **Thanh ghi A:** 93 bit (từ $s_0$ đến $s_{92}$)
*   **Thanh ghi B:** 84 bit (từ $s_{93}$ đến $s_{176}$)
*   **Thanh ghi C:** 111 bit (từ $s_{177}$ đến $s_{287}$)

**Cơ chế cập nhật trạng thái:**
Tại mỗi chu kỳ cập nhật (tương đương một bước lặp sinh bit), thuật toán trích xuất các bit ở những vị trí cố định của 3 thanh ghi, đưa qua các cổng XOR (^) và AND (&) để tạo ra 3 bit phản hồi ($t_1, t_2, t_3$). Các bit này được đẩy chéo vào đầu của thanh ghi tiếp theo (A nhận từ C, B nhận từ A, C nhận từ B). Đồng thời, toàn bộ 288 bit trong mảng được dịch trượt đi 1 vị trí (`memmove`) để tạo không gian trống đón bit mới. Một bit đầu ra ($z$) được sinh ra từ quá trình này để đóng gói thành Keystream.

![Sơ đồ thuật toán Trivium](img/So_do_thuat_toan_trivium.png)`

---

## 2. Hướng dẫn đọc Output của `trace.c` (Theo dõi trạng thái)
Chương trình `trace.c` được thiết kế để khảo sát và giám sát chi tiết trạng thái nội bộ của 288 bit bộ nhớ, đồng thời kiểm chứng tính đúng đắn của công thức toán học. 

### a. Khởi tạo và phân tách thanh ghi (PART 1)
Khi nạp Key và IV (chưa chạy vòng mồi nào), chương trình tách rõ ràng 288 bit thành 3 khối A, B, C. 
-   **Đặc tả chuẩn xác:** Thanh ghi C luôn kết thúc bằng chuỗi `...111`. Thanh ghi A chứa giá trị Key (ghi theo Little-Endian), và thanh ghi B chứa IV. Điều này minh chứng code C đã ánh xạ chính xác 100% đặc tả toán học vào bộ nhớ vật lý.

![Kết quả trace phần 1](img/trace_1.png)

### b. Kiểm chứng công thức sinh bit (PART 2)
Trong 5 vòng đầu tiên, chương trình chứng minh sự kiểm soát hoàn toàn đối với logic lõi:
-   **t1, t2, t3:** Các bit trung gian được trích xuất trước khi mảng bị dịch chuyển.
-   **z_manual:** Bit đầu ra do ta tự tính toán thủ công dựa trên trạng thái hiện tại.
-   **z_system:** Bit đầu ra do cỗ máy `trivium_step()` trả về.
-   **Kết quả `[PASS]`:** Việc `z_manual` luôn bằng `z_system` khẳng định nhóm hoàn toàn làm chủ thuật toán, các vòng lặp dịch bit (`memmove`) hoạt động chính xác mà không làm trôi hay mất dữ liệu.

![Kết quả trace phần 2](img/trace_2.png)

### c. Trạng thái hỗn loạn sau khởi tạo (PART 3 và 4)
Sau khi chạy đủ 1152 vòng khởi tạo , 3 thanh ghi A, B, C từ trạng thái nhiều số 0 đã chuyển sang một mớ dữ liệu xáo trộn ngẫu nhiên cực kỳ phức tạp. Từ trạng thái này, hệ thống mới bắt đầu an toàn để sinh ra luồng Keystream thực sự (16 bit đầu tiên được in ra màn hình).

![Kết quả trace phần 3](img/trace_3.png)

---

## 3. Hiệu ứng tuyết lở (Avalanche Effect) qua `avalanche.c`
**Mục tiêu:** Chứng minh cơ sở khoa học của việc thuật toán Trivium bắt buộc phải thực hiện chính xác 1152 vòng khởi tạo trước khi bắt đầu sinh chuỗi khóa (Keystream). 

Hiệu ứng tuyết lở yêu cầu: Khi thay đổi dù chỉ 1 bit duy nhất của Khóa (Key), luồng dữ liệu mã hóa sinh ra (Keystream) phải thay đổi hoàn toàn khác biệt. Con số lý tưởng trong mật mã học là **~50%** (nghĩa là một nửa số bit bị lật ngược).

Chương trình `avalanche.c` thực hiện: Giữ nguyên IV, dùng 2 Khóa giống hệt nhau nhưng lật đúng 1 bit ở vị trí cuối cùng của Khóa 2. Sau đó sinh ra 1024 bit Keystream và đếm số bit sai khác.

**Bảng số liệu kiểm thử:**
| Số vòng init | Số bit khác biệt (out) | Tỷ lệ khuếch tán (%) | Đánh giá |
| :--- | :--- | :--- | :--- |
| **0** | 353 | 34.47 % | Quá thấp, dữ liệu chưa được trộn đủ, dễ bị bẻ khóa. |
| **288** | 466 | 45.51 % | Bắt đầu xáo trộn sau 1 chu kỳ chạy hết 3 thanh ghi. |
| **576** | 503 | 49.12 % | Đã tiệm cận, nhưng độ an toàn chưa đạt cực đại. |
| **1152** | **517** | **50.49 %** | **Hoàn hảo.** Đạt mức khuếch tán lý tưởng ~50%. |

![Kết quả chạy avalanche.c](img/avalanche.png)

### Kết luận từ số liệu (Vì sao cần 1152 vòng?)
*   Các cổng phi tuyến (toán tử AND `&`) trong Trivium cần một thời gian trễ để các bit thay đổi có thể lan truyền và nhân bản ra khắp 288 vị trí của thanh ghi.
*   Nếu không chạy mồi (0 vòng) hoặc chạy quá ít (288 vòng), hacker có thể dùng toán học tuyến tính để lần ngược lại Khóa gốc từ sự giống nhau của Keystream.
*   Con số **1152 vòng** (bằng đúng 4 chu kỳ trượt toàn bộ 288 bit) là ngưỡng an toàn tuyệt đối. Ở vòng thứ 1152, tỷ lệ thay đổi đã chốt hạ ổn định ở mức **50.49%**, minh chứng cỗ máy Trivium của nhóm đã phá vỡ hoàn toàn mọi tính tuyến tính, tạo ra dòng khóa giả ngẫu nhiên đạt chuẩn mã hóa luồng quốc tế eSTREAM.