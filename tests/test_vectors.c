/*
 * tests/test_vectors.c - Kiem thu Trivium voi test vector chuan (tuan 2)
 * Nguoi viet: Phan Anh Hao
 *
 * Moi vector gom: khoa 80 bit, IV 80 bit, 16 byte keystream dau tien.
 * Nguon: eSTREAM, ban luu trong avr-crypto-lib (xem tests/vectors.txt).
 * Chuong trinh chi GOI ham trong src/trivium.h, khong chua code Trivium.
 * Thoat voi ma 0 neu tat ca PASS, ma 1 neu co vector FAIL (de make test dung).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../src/trivium.h"

#define KS_LEN 16

typedef struct {
    const char *name;       /* ten vector (khop vectors.txt) */
    const char *key_hex;    /* 80 bit = 10 byte = 20 ky tu hex */
    const char *iv_hex;     /* 80 bit */
    const char *ks_hex;     /* 16 byte keystream = 32 ky tu hex */
} test_vector;

static const test_vector VECTORS[] = {
    {"TV1 (All Zeros)",   "00000000000000000000", "00000000000000000000", "FBE0BF265859051B517A2E4E239FC97F"},
    {"TV2 (Key MSB = 1)", "80000000000000000000", "00000000000000000000", "38EB86FF730D7A9CAF8DF13A4420540D"},
    {"TV3 (IV MSB = 1)",  "00000000000000000000", "80000000000000000000", "F8901736640549E3BA7D42EA2D07B9F4"},
    {"TV4 (Ngau nhien)",  "0F62B5085BAE0154A7FA", "288FF65DC42B92F960C7", "A4386C6D7624983FEA8DBE7314E5FE1F"},
};
#define NUM_VECTORS (sizeof(VECTORS) / sizeof(VECTORS[0]))

/* Doi 1 ky tu hex sang gia tri 0..15; tra -1 neu sai. */
static int hex_val(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

/* Doi chuoi hex sang byte (2 ky tu/byte). Tra 0 neu dung, -1 neu sai. */
static int parse_hex(const char *hex, uint8_t *out, size_t outlen)
{
    if (strlen(hex) != 2 * outlen) return -1;
    for (size_t i = 0; i < outlen; i++) {
        int hi = hex_val(hex[2 * i]), lo = hex_val(hex[2 * i + 1]);
        if (hi < 0 || lo < 0) return -1;
        out[i] = (uint8_t)((hi << 4) | lo);
    }
    return 0;
}

static void print_hex(const char *label, const uint8_t *buf, size_t len)
{
    printf("    %s", label);
    for (size_t i = 0; i < len; i++) printf("%02X", buf[i]);
    printf("\n");
}

/* Chay 1 vector: tra 1 neu PASS, 0 neu FAIL. */
static int run_vector(const test_vector *tv)
{
    uint8_t key[10], iv[10], expected[KS_LEN], got[KS_LEN], got2[KS_LEN];
    trivium_ctx ctx;

    if (parse_hex(tv->key_hex, key, sizeof key) != 0 ||
        parse_hex(tv->iv_hex, iv, sizeof iv) != 0 ||
        parse_hex(tv->ks_hex, expected, sizeof expected) != 0) {
        printf("[FAIL] %s: du lieu hex trong test bi sai\n", tv->name);
        return 0;
    }

    /* Kiem tra 1: sinh ca 16 byte mot lan, so voi keystream mong doi */
    trivium_init(&ctx, key, iv);
    trivium_keystream(&ctx, got, KS_LEN);
    int ok_ks = (memcmp(got, expected, KS_LEN) == 0);

    /* Kiem tra 2: sinh tung byte mot, ket qua phai y het (trang thai duoc
       giu dung giua cac lan goi) */
    trivium_init(&ctx, key, iv);
    for (size_t i = 0; i < KS_LEN; i++) trivium_keystream(&ctx, got2 + i, 1);
    int ok_split = (memcmp(got2, expected, KS_LEN) == 0);

    /* Kiem tra 3: ma hoa 16 byte 0 phai ra dung keystream (0 XOR k = k) */
    uint8_t zeros[KS_LEN] = {0}, enc[KS_LEN];
    trivium_init(&ctx, key, iv);
    trivium_crypt(&ctx, zeros, enc, KS_LEN);
    int ok_crypt = (memcmp(enc, expected, KS_LEN) == 0);

    int pass = ok_ks && ok_split && ok_crypt;
    printf("[%s] %s\n", pass ? "PASS" : "FAIL", tv->name);
    if (!pass) {
        print_hex("mong doi : ", expected, KS_LEN);
        print_hex("thuc te  : ", got, KS_LEN);
        if (!ok_split) printf("    (loi khi sinh tung byte)\n");
        if (!ok_crypt) printf("    (loi trivium_crypt)\n");
    }
    return pass;
}

int main(void)
{
    int passed = 0;
    printf("=== Kiem thu Trivium voi %u test vector ===\n", (unsigned)NUM_VECTORS);
    for (size_t i = 0; i < NUM_VECTORS; i++) passed += run_vector(&VECTORS[i]);
    printf("Ket qua: %d/%u vector khop\n", passed, (unsigned)NUM_VECTORS);
    return (passed == (int)NUM_VECTORS) ? 0 : 1;
}
