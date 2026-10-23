/*
 * tests/test_vectors.c - Kiem dinh Trivium bang test vector eSTREAM
 *
 * Nguon vector: avr-crypto-lib, file testvectors/trivium-80.80.test-vectors
 * (chi tiet trong tests/vectors.txt).
 *
 * Quy uoc: Key/IV dua vao dang mang 10 byte, giu dung thu tu hex trong file
 * vector (byte dau tien viet truoc). Keystream 16 byte dau tien.
 *
 * LUU Y cho trivium_init (da kiem chung 4/4 vector PASS bang ban doi chieu):
 * bit thu i (i = 0..79, k_i trong dac ta) cua Key lay tu mang key[] la
 *     k_i = (key[9 - i/8] >> (7 - i%8)) & 1
 * tuc la byte CUOI cua chuoi hex ung voi cac bit DAU cua dac ta, va trong moi
 * byte thi bit cao (MSB) di truoc. IV nap giong het:
 *     v_i = (iv[9 - i/8] >> (7 - i%8)) & 1
 * Nap sai thu tu thi TV1 (toan 0) van PASS, nhung TV2, TV3, TV4 se FAIL.
 *
 * Chuong trinh tra ve 0 neu moi test PASS, khac 0 neu co FAIL
 * (de "make test" bao loi dung).
 */
#include <stdio.h>
#include <string.h>
#include "trivium.h"

#define KS_LEN 16

typedef struct {
    const char *name;
    uint8_t key[TRIVIUM_KEY_BYTES];
    uint8_t iv[TRIVIUM_IV_BYTES];
    uint8_t expected[KS_LEN];
} test_vector;

static const test_vector VECTORS[] = {
    {   /* TV1: Set 2, vector 0 - Key = 0, IV = 0 */
        "TV1",
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0xFB,0xE0,0xBF,0x26,0x58,0x59,0x05,0x1B,
         0x51,0x7A,0x2E,0x4E,0x23,0x9F,0xC9,0x7F}
    },
    {   /* TV2: Set 1, vector 0 - Key MSB = 1, IV = 0 */
        "TV2",
        {0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x38,0xEB,0x86,0xFF,0x73,0x0D,0x7A,0x9C,
         0xAF,0x8D,0xF1,0x3A,0x44,0x20,0x54,0x0D}
    },
    {   /* TV3: Set 5, vector 0 - Key = 0, IV MSB = 1 */
        "TV3",
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        {0xF8,0x90,0x17,0x36,0x64,0x05,0x49,0xE3,
         0xBA,0x7D,0x42,0xEA,0x2D,0x07,0xB9,0xF4}
    },
    {   /* TV4: Set 6, vector 3 - Key va IV "ngau nhien" */
        "TV4",
        {0x0F,0x62,0xB5,0x08,0x5B,0xAE,0x01,0x54,0xA7,0xFA},
        {0x28,0x8F,0xF6,0x5D,0xC4,0x2B,0x92,0xF9,0x60,0xC7},
        {0xA4,0x38,0x6C,0x6D,0x76,0x24,0x98,0x3F,
         0xEA,0x8D,0xBE,0x73,0x14,0xE5,0xFE,0x1F}
    }
};

#define NUM_VECTORS ((int)(sizeof(VECTORS) / sizeof(VECTORS[0])))

static void print_hex(const uint8_t *buf, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++)
        printf("%02X", buf[i]);
}

/* Chay 1 test vector, tra ve 1 neu PASS, 0 neu FAIL. */
static int run_vector(const test_vector *tv)
{
    trivium_ctx ctx;
    uint8_t out[KS_LEN];
    int ok;

    trivium_init(&ctx, tv->key, tv->iv);
    trivium_keystream(&ctx, out, KS_LEN);

    ok = (memcmp(out, tv->expected, KS_LEN) == 0);

    printf("%s: %s\n", tv->name, ok ? "PASS" : "FAIL");
    printf("  Key      : "); print_hex(tv->key, TRIVIUM_KEY_BYTES); printf("\n");
    printf("  IV       : "); print_hex(tv->iv, TRIVIUM_IV_BYTES);   printf("\n");
    printf("  Sinh ra  : "); print_hex(out, KS_LEN);                printf("\n");
    if (!ok) {
        printf("  Mong doi : "); print_hex(tv->expected, KS_LEN);   printf("\n");
    }
    return ok;
}

/* Ma hoa mot chuoi roi giai ma lai, phai ra dung chuoi goc. */
static int run_roundtrip(void)
{
    static const uint8_t key[TRIVIUM_KEY_BYTES] =
        {0x0F,0x62,0xB5,0x08,0x5B,0xAE,0x01,0x54,0xA7,0xFA};
    static const uint8_t iv[TRIVIUM_IV_BYTES] =
        {0x28,0x8F,0xF6,0x5D,0xC4,0x2B,0x92,0xF9,0x60,0xC7};
    const char *msg = "Trivium - Bai tap lon Ly thuyet mat ma, nhom 5";
    size_t len = strlen(msg);
    uint8_t cipher[128], plain[128];
    trivium_ctx ctx;
    int ok;

    /* Ma hoa */
    trivium_init(&ctx, key, iv);
    trivium_crypt(&ctx, (const uint8_t *)msg, cipher, len);

    /* Giai ma: khoi tao lai cung (key, iv) de keystream lap lai */
    trivium_init(&ctx, key, iv);
    trivium_crypt(&ctx, cipher, plain, len);
    plain[len] = '\0';

    ok = (memcmp(plain, msg, len) == 0) && (memcmp(cipher, msg, len) != 0);

    printf("ROUNDTRIP: %s\n", ok ? "PASS" : "FAIL");
    printf("  Ban ro   : %s\n", msg);
    printf("  Ban ma   : "); print_hex(cipher, len); printf("\n");
    printf("  Giai ma  : %s\n", (const char *)plain);
    return ok;
}

int main(void)
{
    int i, passed = 0, total = NUM_VECTORS + 1;

    printf("=== Kiem dinh Trivium (test vector eSTREAM) ===\n");
    for (i = 0; i < NUM_VECTORS; i++)
        passed += run_vector(&VECTORS[i]);
    passed += run_roundtrip();

    printf("--------------------------------------------\n");
    printf("Ket qua: %d/%d PASS\n", passed, total);
    return (passed == total) ? 0 : 1;
}
