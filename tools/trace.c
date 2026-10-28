#include <stdio.h>
#include <stdint.h>
#include "../src/trivium.h"

int main() {
    trivium_ctx ctx;
    uint8_t key[TRIVIUM_KEY_BYTES] = {0}; // Test vector toàn 0
    uint8_t iv[TRIVIUM_IV_BYTES]   = {0};

    // 1. Nạp Key và IV nhưng chạy 0 vòng khởi tạo
    trivium_init_rounds(&ctx, key, iv, 0);

    printf("=== TRANG THAI SAU KHI NAP KEY & IV (0 vong) ===\n");
    for (int i = 0; i < TRIVIUM_STATE_BITS; i++) {
        printf("%d", ctx.s[i]);
        // Tách khoảng trắng giữa 3 thanh ghi A(93), B(84), C(111) cho dễ nhìn
        if (i == 92 || i == (92 + 84)) printf("  "); 
    }
    printf("\n\n");

    // 2. Trace 5 vòng đầu tiên
    printf("=== TRACE 5 VONG DAU TIEN ===\n");
    for (int i = 0; i < 5; i++) {
        // Đọc trực tiếp từ mảng (Do Giang thiết kế mỗi phần tử chứa 1 bit)
        uint8_t s66  = ctx.s[66];
        uint8_t s93  = ctx.s[93];
        uint8_t s162 = ctx.s[162];
        uint8_t s177 = ctx.s[177];
        uint8_t s243 = ctx.s[243];
        uint8_t s288 = ctx.s[288];

        uint8_t t1 = s66 ^ s93;
        uint8_t t2 = s162 ^ s177;
        uint8_t t3 = s243 ^ s288;
        uint8_t z_manual = t1 ^ t2 ^ t3;

        // Gọi hàm của nhóm trưởng
        uint8_t z_system = trivium_step(&ctx);

        printf("Vong %d: t1=%d, t2=%d, t3=%d | z_manual=%d, z_system=%d -> ", 
               i + 1, t1, t2, t3, z_manual, z_system);
        
        if (z_manual == z_system) printf("[PASS]\n");
        else printf("[FAIL]\n");
    }

    // 3. Trạng thái sau 1152 vòng khởi tạo
    printf("\n=== TRANG THAI SAU 1152 VONG KHOI TAO ===\n");
    trivium_init_rounds(&ctx, key, iv, TRIVIUM_INIT_ROUNDS);
    for (int i = 0; i < TRIVIUM_STATE_BITS; i++) {
        printf("%d", ctx.s[i]);
        if (i == 92 || i == (92 + 84)) printf("  ");
    }
    printf("\n\n");

    // 4. Sinh 16 bit keystream đầu tiên
    printf("=== 16 BIT KEYSTREAM DAU TIEN ===\n");
    for (int i = 0; i < 16; i++) {
        printf("%d", trivium_step(&ctx));
        if ((i + 1) % 8 == 0) printf(" ");
    }
    printf("\n\n");

    return 0;
}