import sys
import os

# Khai báo đường dẫn để thư mục tests có thể nhìn thấy thư mục src
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'src')))


from anhngoc import Trivium

print("BẮT ĐẦU CHẠY THỬ TRIVIUM")

my_trivium = Trivium()

print("5 bit đầu của Thanh ghi 1:", my_trivium.reg1[:5])
print("Kết nối code thành công! Sẵn sàng nạp test_vectors.txt sau khi viết xong thuật toán.")
