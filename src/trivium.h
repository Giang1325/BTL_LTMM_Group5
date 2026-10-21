#ifndef TRIVIUM_H
#define TRIVIUM_H

#include <stddef.h>
#include <stdint.h>

#define TRIVIUM_KEY_BYTES   10      /* 80 bit */
#define TRIVIUM_IV_BYTES    10      /* 80 bit */
#define TRIVIUM_STATE_BITS  288
#define TRIVIUM_INIT_ROUNDS 1152    /* 4 * 288 vong khoi tao */

typedef struct {
    uint8_t s[TRIVIUM_STATE_BITS];
} trivium_ctx;

void trivium_init(trivium_ctx *ctx, const uint8_t key[TRIVIUM_KEY_BYTES],
                  const uint8_t iv[TRIVIUM_IV_BYTES]);

void trivium_init_rounds(trivium_ctx *ctx, const uint8_t key[TRIVIUM_KEY_BYTES],
                         const uint8_t iv[TRIVIUM_IV_BYTES], int rounds);

int trivium_step(trivium_ctx *ctx);

void trivium_keystream(trivium_ctx *ctx, uint8_t *out, size_t len);

void trivium_crypt(trivium_ctx *ctx, const uint8_t *in, uint8_t *out, size_t len);

#endif
