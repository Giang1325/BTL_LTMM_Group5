#ifndef HEX_H
#define HEX_H

#include <stddef.h>
#include <stdint.h>


int hex_to_bytes(const char *hex, uint8_t *out, size_t out_len);

void bytes_to_hex(const uint8_t *in, size_t in_len, char *out);

void print_hex(const uint8_t *in, size_t in_len);

#endif 
