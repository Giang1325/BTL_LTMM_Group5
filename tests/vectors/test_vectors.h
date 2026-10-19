#ifndef TEST_VECTORS_H
#define TEST_VECTORS_H

// Cấu trúc mô tả một Test Vector Trivium (chiều dài dòng khóa: 16 bytes)
typedef struct {
    const char *id;
    const char *source;
    const char *key_hex;
    const char *iv_hex;
    const char *keystream_hex;
} TriviumTestVector;

// Số lượng vector
#define NUM_VECTORS 8

// Mảng chứa các test vector chuẩn
extern const TriviumTestVector trivium_vectors[NUM_VECTORS];

#endif // TEST_VECTORS_H