
#include <stdio.h>
#include <stdint.h>
#include "../src/trivium.h"

#define KEYSTREAM_LEN 1024

// Kiểm tra hiệu ứng tuyết lở với số vòng khởi tạo cho trươc
void run_avalanche_test(int rounds) 
{
    trivium_ctx ctx1, ctx2;  // ctx1 là trạng thái của mảng s[288] thanh ghi trong lần chạy thứ nhất
    
    // Hai khóa ban đầu giống nhau, đều bằng 0
    uint8_t key1[TRIVIUM_KEY_BYTES] = {0};
    uint8_t key2[TRIVIUM_KEY_BYTES] = {0};

    // Hai lần thử sử dụng cùng một IV
    uint8_t iv[TRIVIUM_IV_BYTES] = {0};

    // Lật 1 bit tại vị trí thấp nhất của byte key[0]
    key2[0] = 1;

    // Khởi tạo hai trạng thái với cùng số vòng
    trivium_init_rounds(&ctx1, key1, iv, rounds);
    trivium_init_rounds(&ctx2, key2, iv, rounds);

    int diff_count = 0; // đếm số vị trí bit đầu ra khác nhau giữa hai Keystream

    // Sinh và so sánh 1024 bit keystream
    for (int i = 0; i < KEYSTREAM_LEN; i++)
    {
        int bit1 = trivium_step(&ctx1);
        int bit2 = trivium_step(&ctx2);

        // Đếm số vị trí có bit đầu ra khác nhau
        if (bit1 != bit2)
        {
            diff_count++;
        }
    }

    // Tính tỷ lệ bit khác biệt (%)
    float percentage = ((float)diff_count / KEYSTREAM_LEN) * 100.0f;

    // In kết quả
    printf("| %-12d | %-22d | %-12.2f%% |\n",
           rounds, diff_count, percentage);
}

int main(void)
{
    printf("===============================================\n");
    printf("     TRIVIUM - AVALANCHE EFFECT TEST\n");
    printf("===============================================\n");

    printf("Thay doi 1 bit cua Key, giu nguyen IV.\n");
    printf("Moi truong hop so sanh %d bit keystream.\n\n",
           KEYSTREAM_LEN);

    printf("| So vong init | So bit khac biet (out) | Ty le       |\n");
    printf("|--------------|------------------------|-------------|\n");

    // Các mức số vòng khởi tạo cần kiểm tra
    int test_rounds[] = {0, 288, 576, 1152};

    int num_tests = sizeof(test_rounds) / sizeof(test_rounds[0]); 
    // tự động tính toán số lượng phần tử đang có trong một mảng
    for (int i = 0; i < num_tests; i++)
    {
        run_avalanche_test(test_rounds[i]);
    }

    printf("\nKet thuc kiem tra Avalanche Effect.\n");

    return 0;
}
