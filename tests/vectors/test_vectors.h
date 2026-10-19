#ifndef TEST_VECTORS_H
#define TEST_VECTORS_H

// Cấu trúc mô tả một Test Vector Trivium (chiều dài dòng khóa: 32 bytes)
typedef struct {
    const char *source;
    const char *key_hex;
    const char *iv_hex;
    const char *keystream_hex;
} TriviumTestVector;

#define NUM_VECTORS 4
extern const TriviumTestVector trivium_vectors[NUM_VECTORS];

#endif // TEST_VECTORS_H