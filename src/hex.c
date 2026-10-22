#include "hex.h"
#include <stdio.h>
#include <string.h>

/* Ham phu: doi 1 ky tu hex sang gia tri 0..15, tra ve -1 neu ky tu la */
static int hex_char_to_val(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

int hex_to_bytes(const char *hex, uint8_t *out, size_t out_len) {
    if (hex == NULL || out == NULL) {
        return -1;
    }

    size_t hex_len = strlen(hex);
    if (hex_len != out_len * 2) {
        return -1; /* Sai do dai */
    }

    for (size_t i = 0; i < out_len; i++) {
        int hi = hex_char_to_val(hex[i * 2]);
        int lo = hex_char_to_val(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) {
            return -1; /* Co ky tu khong phai hex */
        }
        out[i] = (uint8_t)((hi << 4) | lo);
    }

    return 0;
}

void bytes_to_hex(const uint8_t *in, size_t in_len, char *out) {
    static const char hex_digits[] = "0123456789ABCDEF";
    for (size_t i = 0; i < in_len; i++) {
        out[i * 2]     = hex_digits[(in[i] >> 4) & 0x0F];
        out[i * 2 + 1] = hex_digits[in[i] & 0x0F];
    }
    out[in_len * 2] = '\0';
}

void print_hex(const uint8_t *in, size_t in_len) {
    for (size_t i = 0; i < in_len; i++) {
        printf("%02X", in[i]);
    }
    printf("\n");
}
