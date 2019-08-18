/* Seiran128 v1 - pseudorandom number generator

To the extent possible under law, the author has waived all copyright 
and related or neighboring rights to this software.
See: https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <inttypes.h>

// rotate left (ROL) instruction
static inline uint64_t rotl(uint64_t x, int k)
{
    return (x << k) | (x >> (-k & 0x3f));
}

static uint64_t state[2];

// Returns 64-bit pseudorandom value.
uint64_t next(void)
{
    uint64_t s0 = state[0], s1 = state[1];
    uint64_t result = rotl((s0 + s1) * 9, 29) + s0;

    state[0] = s0 ^ rotl(s1, 29);
    state[1] = s0 ^ (s1 << 9);

    return result;
}

// Initialize state.
void init(uint64_t seed)
{
    // You may initialize state by any method, unless state will be {0, 0}.
    for (int i = 0; i < 2; i++)
    {
        state[i] = seed = seed * 6364136223846793005 + 1442695040888963407;
    }
}

static inline void jump(const uint64_t jumppoly[])
{
    uint64_t t[2] = {0, 0};

    for (int i = 0; i < 2; i++)
    {
        for (int b = 0; b < 64; b++)
        {
            if ((jumppoly[i] >> b) & 1)
            {
                t[0] ^= state[0];
                t[1] ^= state[1];
            }
            next();
        }
    }

    state[0] = t[0];
    state[1] = t[1];
}

// It is equivalent to 2^32 calls of next().
void jump32(void)
{
    const uint64_t jumppoly_2_32[] = {0x40165CBAE9CA6DEB, 0x688E6BFC19485AB1};
    jump(jumppoly_2_32);
}

// It is equivalent to 2^64 calls of next().
void jump64(void)
{
    const uint64_t jumppoly_2_64[] = {0xF4DF34E424CA5C56, 0x2FE2DE5C2E12F601};
    jump(jumppoly_2_64);
}

// It is equivalent to 2^96 calls of next().
void jump96(void)
{
    const uint64_t jumppoly_2_96[] = {0x185F4DF8B7634607, 0x95A98C7025F908B2};
    jump(jumppoly_2_96);
}

//////// The test code is shown below. ////////

#include <stdio.h>

#define ASSERT(pred)               \
    if (!(pred))                   \
    {                              \
        puts("assertion failed."); \
        return -1;                 \
    }
#define PRINTSTATE(title) printf(title "%016" PRIx64 " %016" PRIx64 "\n", state[0], state[1])

int main(void)
{
    init(401);
    PRINTSTATE("init: ");
    ASSERT(state[0] == 0x6C64F673ED93B6CC && state[1] == 0x97C703D5F6C9D72B);

    printf("next: ");
    {
        const uint64_t outputs[] = {
            0x8D4E3629D245305F, 0x941C2B08EB30A631,
            0x4246BDC17AD8CA1E, 0x5D5DA3E87E82EB7C};
        for (int i = 0; i < 4; i++)
        {
            uint64_t value = next();
            printf("%016" PRIx64 " ", value);
            ASSERT(value == outputs[i]);
        }
    }
    puts("");

    PRINTSTATE("jp 0: ");
    ASSERT(state[0] == 0x8B38637963B4B7A0 && state[1] == 0x74A5A5F4B505E908);

    jump32();
    PRINTSTATE("jp32: ");
    ASSERT(state[0] == 0xA20E518250A6FF34 && state[1] == 0x6F80EE02ACF8EBC2);

    jump64();
    PRINTSTATE("jp64: ");
    ASSERT(state[0] == 0x38CB1C2C7821B08F && state[1] == 0xD518DCC9FA1C92BA);

    jump96();
    PRINTSTATE("jp96: ");
    ASSERT(state[0] == 0x77AF4FDC1B861F98 && state[1] == 0x16485EFAC22AE75B);

    puts("succeeded.");
    return 0;
}
