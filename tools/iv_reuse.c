/*
 * tools/iv_reuse.c - Demo tan cong dung lai (Key, IV) cua ma dong (two-time pad)
 *
 * Buoc 1: ma hoa 2 cau P1, P2 bang CUNG Key va IV  -> C1, C2.
 * Buoc 2: ke tan cong chi thay C1, C2:  C1 ^ C2 == P1 ^ P2 (keystream bi triet tieu).
 * Buoc 3: biet them P1 (known-plaintext) -> P2 = (C1 ^ C2) ^ P1, KHONG can Key.
 * Doi chung: doi IV cho thong diep thu hai thi hai dang thuc tren khong con dung.
 */
#include <stdio.h>
#include <string.h>
#include "trivium.h"

#define MAX_LEN 128

static void print_hex(const char *label, const uint8_t *buf, size_t len)
{
    size_t i;
    printf("%-26s", label);
    for (i = 0; i < len; i++)
        printf("%02X", buf[i]);
    printf("\n");
}

/* In byte dang chuoi; byte khong in duoc thay bang '.' */
static void print_text(const char *label, const uint8_t *buf, size_t len)
{
    size_t i;
    printf("%-26s", label);
    for (i = 0; i < len; i++)
        putchar((buf[i] >= 32 && buf[i] < 127) ? buf[i] : '.');
    printf("\n");
}

static void xor_bytes(const uint8_t *a, const uint8_t *b, uint8_t *out, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++)
        out[i] = a[i] ^ b[i];
}

int main(void)
{
    static const uint8_t key[TRIVIUM_KEY_BYTES] =
        {0x0F,0x62,0xB5,0x08,0x5B,0xAE,0x01,0x54,0xA7,0xFA};
    static const uint8_t iv[TRIVIUM_IV_BYTES] =
        {0x28,0x8F,0xF6,0x5D,0xC4,0x2B,0x92,0xF9,0x60,0xC7};
    static const uint8_t iv_other[TRIVIUM_IV_BYTES] =
        {0x28,0x8F,0xF6,0x5D,0xC4,0x2B,0x92,0xF9,0x60,0xC8};   /* khac 1 bit cuoi */

    const char *p1_str = "Chuyen 500 trieu vao tai khoan 1234 sang mai";
    const char *p2_str = "Mat khau wifi phong lab la Trivium@2026 nhe!";

    uint8_t p1[MAX_LEN], p2[MAX_LEN], c1[MAX_LEN], c2[MAX_LEN];
    uint8_t c1_xor_c2[MAX_LEN], p1_xor_p2[MAX_LEN], recovered[MAX_LEN];
    uint8_t c2_safe[MAX_LEN], safe_xor[MAX_LEN];
    size_t n1 = strlen(p1_str), n2 = strlen(p2_str);
    size_t n = (n1 < n2) ? n1 : n2;     /* chi so sanh phan chung do dai */
    trivium_ctx ctx;
    int ok_cancel, ok_recover, ok_control;

    memcpy(p1, p1_str, n1);
    memcpy(p2, p2_str, n2);

    printf("=== Tan cong dung lai Key/IV (two-time pad) tren Trivium ===\n\n");

    /* Buoc 1: ma hoa 2 cau bang cung Key + IV (khoi tao lai de keystream lap lai) */
    trivium_init(&ctx, key, iv);
    trivium_crypt(&ctx, p1, c1, n1);
    trivium_init(&ctx, key, iv);
    trivium_crypt(&ctx, p2, c2, n2);

    printf("[1] Ma hoa 2 thong diep bang CUNG Key va IV\n");
    print_text("P1 (ban ro 1):", p1, n1);
    print_text("P2 (ban ro 2):", p2, n2);
    print_hex ("C1 (ban ma 1):", c1, n1);
    print_hex ("C2 (ban ma 2):", c2, n2);

    /* Buoc 2: C1 ^ C2 == P1 ^ P2 */
    xor_bytes(c1, c2, c1_xor_c2, n);
    xor_bytes(p1, p2, p1_xor_p2, n);
    ok_cancel = (memcmp(c1_xor_c2, p1_xor_p2, n) == 0);

    printf("\n[2] Keystream bi triet tieu: C1 xor C2 = P1 xor P2 ?\n");
    print_hex("C1 xor C2:", c1_xor_c2, n);
    print_hex("P1 xor P2:", p1_xor_p2, n);
    printf("Ket qua: %s\n", ok_cancel ? "TRUNG KHOP (keystream da mat)" : "KHONG KHOP");

    /* Buoc 3: biet P1 -> khoi phuc P2 khong can Key */
    xor_bytes(c1_xor_c2, p1, recovered, n);
    ok_recover = (memcmp(recovered, p2, n) == 0);

    printf("\n[3] Ke tan cong biet P1, chi co C1, C2 (KHONG co Key, KHONG co keystream)\n");
    printf("    P2 = (C1 xor C2) xor P1\n");
    print_text("P2 khoi phuc duoc:", recovered, n);
    print_text("P2 that:", p2, n);
    printf("Ket qua: %s\n", ok_recover ? "KHOI PHUC DUNG NGUYEN VAN P2" : "SAI");

    /* Doi chung: IV khac cho thong diep thu hai */
    trivium_init(&ctx, key, iv_other);
    trivium_crypt(&ctx, p2, c2_safe, n2);
    xor_bytes(c1, c2_safe, safe_xor, n);
    ok_control = (memcmp(safe_xor, p1_xor_p2, n) != 0);

    printf("\n[4] Doi chung: dung IV khac (chi khac 1 bit) cho P2\n");
    print_hex("C1 xor C2':", safe_xor, n);
    print_hex("P1 xor P2:", p1_xor_p2, n);
    printf("Ket qua: %s\n", ok_control ? "KHONG KHOP -> tan cong that bai (IV phai duy nhat)"
                                       : "VAN KHOP (bat thuong!)");

    printf("\nTong ket: %s\n",
           (ok_cancel && ok_recover && ok_control) ? "DEMO THANH CONG" : "DEMO CO LOI");
    return (ok_cancel && ok_recover && ok_control) ? 0 : 1;
}
