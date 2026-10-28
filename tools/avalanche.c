#include <stdio.h>
#include <stdint.h>
#include "../src/trivium.h"

#define KEYSTREAM_LEN 1024

void run_avalanche_test(int rounds) {
    trivium_ctx ctx1, ctx2;
    uint8_t key1[TRIVIUM_KEY_BYTES] = {0};
    uint8_t key2[TRIVIUM_KEY_BYTES] = {0};
    uint8_t iv[TRIVIUM_IV_BYTES]    = {0};
    
    // Lật 1 bit đầu tiên của khóa 2 để đánh giá sự phân tán
    key2[0] = 0x01; 

    // Dùng đúng hàm và cấu trúc tham số của Giang
    trivium_init_rounds(&ctx1, key1, iv, rounds);
    trivium_init_rounds(&ctx2, key2, iv, rounds);

    int diff_count = 0;
    for (int i = 0; i < KEYSTREAM_LEN; i++) {
        int bit1 = trivium_step(&ctx1);
        int bit2 = trivium_step(&ctx2);
        if (bit1 != bit2) {
            diff_count++;
        }
    }

    float percentage = (diff_count / KEYSTREAM_LEN) * 100.0f;
    printf("| %-12d | %-22d | %-12.2f |\n", rounds, diff_count, percentage);
}

int main() {
    printf("=== DANH GIA HIEU UNG TUYET LO (AVALANCHE EFFECT) ===\n");
    printf("Thay doi 1 bit khoa, sinh %d bit Keystream\n\n", KEYSTREAM_LEN);
    printf("| So vong init | So bit khac biet (out) | Ty le (%%)    |\n");
    printf("|--------------|------------------------|--------------|\n");
    
    int test_rounds[] = {0, 288, 576, 1152};
    for (int i = 0; i < 4; i++) {
        run_avalanche_test(test_rounds[i]);
    }
    printf("\n");
    return 0;
}