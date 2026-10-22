#ifndef HEX_H
#define HEX_H

#include <stddef.h>
#include <stdint.h>

/*
 * Chuyen chuoi hex sang mang byte.
 * - hex: chuoi ky tu hex dau vao (do dai phai bang out_len * 2)
 * - out: mang byte dau ra
 * - out_len: so byte mong muon
 * Tra ve: 0 neu thanh cong, -1 neu sai do dai hoac co ky tu la.
 */
int hex_to_bytes(const char *hex, uint8_t *out, size_t out_len);

/*
 * Chuyen mang byte sang chuoi hex (viet hoa).
 * - in: mang byte dau vao
 * - in_len: so byte
 * - out: chuoi ky tu dau ra (kich thuoc toi thieu in_len * 2 + 1)
 */
void bytes_to_hex(const uint8_t *in, size_t in_len, char *out);

/*
 * In mang byte ra man hinh duoi dang chuoi hex viet hoa.
 */
void print_hex(const uint8_t *in, size_t in_len);

#endif /* HEX_H */
