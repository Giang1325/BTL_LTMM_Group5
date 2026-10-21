#include <string.h>
#include "trivium.h"

void trivium_init(trivium_ctx *ctx, const uint8_t key[TRIVIUM_KEY_BYTES],
                  const uint8_t iv[TRIVIUM_IV_BYTES])
{
    trivium_init_rounds(ctx, key, iv, TRIVIUM_INIT_ROUNDS);
}

void trivium_init_rounds(trivium_ctx *ctx, const uint8_t key[TRIVIUM_KEY_BYTES],
                         const uint8_t iv[TRIVIUM_IV_BYTES], int rounds)
{
    (void)key;
    (void)iv;
    (void)rounds;
    memset(ctx->s, 0, sizeof(ctx->s));
}

int trivium_step(trivium_ctx *ctx)
{
    (void)ctx;
    return 0;
}

void trivium_keystream(trivium_ctx *ctx, uint8_t *out, size_t len)
{
    size_t i;
    int j;

    for (i = 0; i < len; i++) {
        out[i] = 0;
        for (j = 0; j < 8; j++)
            out[i] |= (uint8_t)(trivium_step(ctx) << j);   /* LSB truoc */
    }
}

void trivium_crypt(trivium_ctx *ctx, const uint8_t *in, uint8_t *out, size_t len)
{
    size_t i;
    uint8_t k;

    for (i = 0; i < len; i++) {
        trivium_keystream(ctx, &k, 1);
        out[i] = in[i] ^ k;
    }
}
