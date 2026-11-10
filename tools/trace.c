#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../src/trivium.h"

// In trang thai 288 bit, chia thanh 3 thanh ghi A, B, C
void print_state(trivium_ctx *ctx)
{
    printf("  [A] (93 bits)  : ");
    for (int i = 0; i < 93; i++) {
        printf("%d", ctx->s[i]);
    }
    printf("\n");

    printf("  [B] (84 bits)  : ");
    for (int i = 93; i < 177; i++) {
        printf("%d", ctx->s[i]);
    }
    printf("\n");

    printf("  [C] (111 bits) : ");
    for (int i = 177; i < 288; i++) {
        printf("%d", ctx->s[i]);
    }
    printf("\n");
}

// Tinh va kiem tra bit dau ra z tu trang thai hien tai
void trace_one_round(trivium_ctx *ctx, int round)
{
    // Đọc trạng thái cũ và tính t1, t2, t3
    uint8_t t1 = ctx->s[65] ^ ctx->s[92];
    uint8_t t2 = ctx->s[161] ^ ctx->s[176];
    uint8_t t3 = ctx->s[242] ^ ctx->s[287];

    // Tính bit đầu ra z thủ công
    uint8_t z_manual = t1 ^ t2 ^ t3;

    // Goi ham step de lay bit dau ra va cap nhat trang thai
    uint8_t z_system = trivium_step(ctx);

    printf("\n--- VONG %d ---\n", round);
    printf("  t1 = %d\n", t1);
    printf("  t2 = %d\n", t2);
    printf("  t3 = %d\n", t3);
    printf("  z_manual = %d\n", z_manual);
    printf("  z_system = %d\n", z_system);

    // So sanh ket qua tinh thu cong va ham trivium_step()
    if (z_manual == z_system) {
        printf("  Kiem tra bit z: [PASS]\n");
    } else {
        printf("  Kiem tra bit z: [FAIL]\n");
    }

    // Trang thai luc nay da duoc cap nhat sau mot vong
    printf("  Trang thai sau vong %d:\n", round);
    print_state(ctx);
}

int main(void) // hàm chính trả về 1 số nguyên và không nhận tham số đầu vào
{
    trivium_ctx ctx; // tạo biến ctx thuộc kiểu dữ liệu trivum_ctx suy ra ctx chưa mảng s gồm 288 phần tử

    uint8_t key[TRIVIUM_KEY_BYTES] = {0};
    uint8_t iv[TRIVIUM_IV_BYTES] = {0};

    printf("========================================\n");
    printf("       TRIVIUM STATE TRACE PROGRAM\n");
    printf("========================================\n");

    // PART 1: Nap Key va IV, chua thuc hien vong khoi tao
    printf("\n[PART 1] TRANG THAI BAN DAU\n");

    // Thay doi mot bit cua Key de de quan sat trang thai
    key[0] = 1;

    // Khoi tao trang thai voi 0 vong
    trivium_init_rounds(&ctx, key, iv, 0);

    printf("Trang thai sau khi nap Key va IV (0 vong):\n");
    print_state(&ctx);

    // PART 2: Theo doi 5 vong dau tien
    printf("\n[PART 2] THEO DOI 5 VONG DAU TIEN\n");

    for (int i = 0; i < 5; i++) {
        trace_one_round(&ctx, i + 1);
    }

    // PART 3: Khoi tao lai va thuc hien 1152 vong lam nong
    printf("\n[PART 3] TRANG THAI SAU 1152 VONG KHOI TAO\n");

    // Khoi tao lai tu dau, tranh tiep tuc tu trang thai da trace 5 vong
    trivium_init_rounds(&ctx, key, iv, TRIVIUM_INIT_ROUNDS);

    printf("Trang thai sau %d vong khoi tao:\n", TRIVIUM_INIT_ROUNDS);
    print_state(&ctx);

    // PART 4: Sinh 16 bit keystream dau tien
    printf("\n[PART 4] 16 BIT KEYSTREAM DAU TIEN\n");

    for (int i = 0; i < 16; i++) {
        printf("%d", trivium_step(&ctx));

        if ((i + 1) % 8 == 0) {
            printf(" "); //  cứ sau 8 bit thì in dấu cách để dễ quan sát
        }
    }

    printf("\n\nKet thuc chuong trinh.\n");

    return 0;
}
