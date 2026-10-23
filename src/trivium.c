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
    int i;

    memset(ctx->s, 0, sizeof(ctx->s));

    for (i = 0; i < 80; i++) {
        ctx->s[i]      = (key[9 - i / 8] >> (7 - i % 8)) & 1;
        ctx->s[93 + i] = (iv[9 - i / 8] >> (7 - i % 8)) & 1;
    }

    /*s286=s287=s288=1*/
    ctx->s[285] = 1;
    ctx->s[286] = 1;
    ctx->s[287] = 1;

    for (i = 0; i < rounds; i++)
        trivium_step(ctx);
}

int trivium_step(trivium_ctx *ctx)
{
    uint8_t *s = ctx->s;
    uint8_t t1, t2, t3, z;

    t1 = s[65] ^ s[92];         /*t1=s66+s93 */
    t2 = s[161] ^ s[176];       /*t2=s162+s177*/
    t3 = s[242] ^ s[287];       /*t3=s243+s288*/
    z = t1 ^ t2 ^ t3;

    t1 ^= (s[90] & s[91]) ^ s[170];     /*+s91*s92+s171*/
    t2 ^= (s[174] & s[175]) ^ s[263];   /*+s175*s176+s264*/
    t3 ^= (s[285] & s[286]) ^ s[68];    /*+s286*s287+s69*/

    memmove(&s[1], &s[0], 92);
    s[0] = t3;
    memmove(&s[94], &s[93], 83);
    s[93] = t1;
    memmove(&s[178], &s[177], 110);
    s[177] = t2;

    return z;
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