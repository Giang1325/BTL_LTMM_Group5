# Bài tập lớn: Lý thuyết mật mã — Nhóm 5

## Đề tài
Tìm hiểu, cài đặt và kiểm thử thuật toán mã dòng **Trivium**.

## Giới thiệu
Trivium là một thuật toán mã dòng đồng bộ, được thiết kế bởi Christophe De Cannière và Bart Preneel, là một trong các ứng viên profile phần cứng (hardware-oriented) của dự án eSTREAM. Thuật toán sử dụng khóa (key) 80 bit và vector khởi tạo (IV) 80 bit, sinh keystream từ 3 thanh ghi dịch hồi tiếp phi tuyến (NFSR) kết hợp với nhau.

## Nội dung thực hiện
- Trình bày lý thuyết: cấu trúc thuật toán, quá trình khởi tạo, sinh keystream.
- Cài đặt chương trình mã hóa/giải mã bằng Trivium.
- Chạy kiểm thử với các test vector chuẩn để xác minh tính đúng đắn.

## Thành viên nhóm
- Họ tên 1 — MSSV
- Nguyễn Trường Giang — 20233375
- Đồng Vũ Ngọc Anh — 20233243
- Phan Anh Hào — 20233386

## Cấu trúc thư mục
- `/src`: mã nguồn cài đặt thuật toán
- `/tests`: test vector và unit test
- `/docs`: báo cáo, tài liệu
