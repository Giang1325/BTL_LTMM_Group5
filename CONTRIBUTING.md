# Quy tắc làm việc chung — Nhóm 5

BTL Lý thuyết mật mã — Cài đặt và kiểm định mã dòng Trivium.
Mọi thành viên đọc file này trước khi đẩy code lần đầu.

## 1. Môi trường

- **Ngôn ngữ:** C, chuẩn C99. Toàn bộ mã trong `src/`, `tests/`, `tools/` viết bằng C, không thêm code ngôn ngữ khác.
- **Trình biên dịch:** GCC (MinGW-w64 trên Windows hoặc WSL/Ubuntu).
- **Lệnh build:** trên Windows/MinGW gõ `mingw32-make`, trên WSL/Linux gõ `make`.

| Lệnh         | Tác dụng                      |
|--------------|-------------------------------|
| `make`       | Build chương trình            |
| `make test`  | Chạy toàn bộ kiểm thử         |
| `make clean` | Xóa các file đã build         |

## 2. Năm quy tắc bắt buộc

1. **Không commit hay push thẳng lên `main`.** Mọi thay đổi vào `main` phải qua Pull Request.
2. **Mỗi việc một nhánh riêng**, tạo từ `main` mới nhất.
3. **Trước khi mở Pull Request:** `make` không có cảnh báo và `make test` chạy qua.
4. **Không commit file sinh ra khi build hoặc chạy** (`*.o`, `*.exe`, `__pycache__/`...), file thử hay file cá nhân.
5. **Muốn sửa file thuộc module của người khác thì báo chủ module trước** (xem mục 6).

## 3. Quy trình cho mỗi việc

```bash
# 1. Cap nhat main moi nhat
git checkout main
git pull

# 2. Tao nhanh moi cho viec cua minh
git checkout -b feature/ten-viec

# 3. Code xong thi build va test
make
make test

# 4. Luu thay doi (chi add dung file minh sua)
git add src/hex.c src/hex.h
git commit -m "feat: them ham hex_to_bytes"

# 5. Day nhanh len GitHub
git push -u origin feature/ten-viec
```

6. Lên GitHub, mở **Pull Request** từ nhánh của mình vào `main`.
7. Người review đọc và nhận xét. Nếu cần sửa, cứ commit tiếp trên nhánh cũ rồi push lại; Pull Request tự cập nhật.
8. Khi đã được duyệt, Giang merge và xóa nhánh trên GitHub. Mọi người về máy chạy `git checkout main` rồi `git pull`.

### Đặt tên nhánh

Dạng `<loai>/<mo-ta-ngan>`: chữ thường, không dấu, nối bằng gạch ngang.

| Tiền tố     | Dùng khi                  | Ví dụ                       |
|-------------|---------------------------|-----------------------------|
| `feature/`  | Thêm chức năng            | `feature/hex-module`        |
| `fix/`      | Sửa lỗi                   | `fix/lech-chi-so-t2`        |
| `test/`     | Thêm hoặc sửa kiểm thử    | `test/test-vectors`         |
| `docs/`     | Tài liệu, báo cáo         | `docs/tom-tat-trivium`      |

## 4. Commit message

Dạng `<loai>: <mo ta ngan>`, viết **không dấu** (tránh lỗi font trên terminal Windows), tối đa khoảng 70 ký tự, nói rõ đã làm gì.

Loại: `feat` (chức năng mới), `fix` (sửa lỗi), `test` (kiểm thử), `docs` (tài liệu), `build` (Makefile, cấu hình).

- Tốt: `feat: them ham hex_to_bytes`, `fix: sua chi so t2 lech 1 bit`, `test: them TV5-TV8`
- Không tốt: `update`, `sua`, `abc`, `final`

Mỗi commit nên là một thay đổi có ý nghĩa; đừng gộp nhiều việc không liên quan vào một commit.

## 5. Pull Request

- **Tiêu đề:** viết như commit message.
- **Mô tả:** làm gì, đã kiểm tra thế nào (đã chạy `make test` chưa, kết quả ra sao), còn gì chưa xong.
- **Nhỏ và sớm:** mỗi Pull Request một việc. Mở sớm để người khác thấy tiến độ.
- **Duyệt:** chỉ merge khi có ít nhất 1 người **không phải tác giả** đã duyệt.

## 6. Chủ sở hữu module

Mỗi file có một người chịu trách nhiệm chính. Khi có xung đột (conflict) trong file của ai, người đó quyết định giữ phần nào.

| File / thư mục                                                   | Chủ sở hữu |
|------------------------------------------------------------------|------------|
| `Makefile`, `CONTRIBUTING.md`, `tools/bench.c`, `docs/` (ghép)  | Giang      |
| `src/trivium.h`, `src/trivium.c`, `src/trivium64.c`              | Hào        |
| `tests/vectors/`, `tests/*.c`                                    | Bảo        |
| `src/hex.h`, `src/hex.c`, `src/main.c`, `tools/avalanche.c`      | Ngọc Anh   |

## 7. Viết code

- Thụt dòng trong file C bằng 4 dấu cách. **Riêng Makefile bắt buộc dùng Tab.**
- Tên hàm, biến viết thường nối gạch dưới (`trivium_init`), theo đúng interface đã chốt trong `src/trivium.h`.
- Code lõi thuật toán có comment đối chiếu với từng dòng trong đặc tả Trivium.
- Không để lại cảnh báo khi build với `-Wall -Wextra`.