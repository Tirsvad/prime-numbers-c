#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

#define LIMIT 1000000u

// Bit i of the sieve stands for the odd number 2i + 1, so 1,000,001 numbers need
// only LIMIT / 2 + 1 bits (about 62 KB), which fits in the CPU cache.
#define SIEVE_BITS (LIMIT / 2 + 1)
#define SIEVE_WORDS ((SIEVE_BITS + 63) / 64)

// Writes every prime from 0 to LIMIT into out and returns how many were found.
// Sieve of Eratosthenes over odd numbers: a set bit means "composite".
static size_t sieve_primes(uint32_t *out) {
    static uint64_t composite[SIEVE_WORDS];
    memset(composite, 0, sizeof composite);

    // Cross out odd multiples of each odd prime p, starting at p * p (smaller
    // multiples already have a smaller prime factor). p * p fits in 64 bits.
    for (uint64_t p = 3; p * p <= LIMIT; p += 2) {
        if (composite[(p >> 1) >> 6] >> ((p >> 1) & 63) & 1) continue; // p is composite
        // Consecutive odd multiples of p are 2p apart, i.e. p apart in bit index.
        for (uint64_t m = (p * p) >> 1; m < SIEVE_BITS; m += p)
            composite[m >> 6] |= 1ull << (m & 63);
    }

    size_t count = 0;
    if (LIMIT >= 2) out[count++] = 2; // the only even prime
    for (uint32_t i = 1; i < SIEVE_BITS; i++) {
        uint32_t n = 2 * i + 1;
        if (n > LIMIT) break;
        if (!(composite[i >> 6] >> (i & 63) & 1)) out[count++] = n;
    }
    return count;
}

// Seconds elapsed between two timespec_get(TIME_UTC) samples.
static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (double)(b->tv_sec - a->tv_sec) + (double)(b->tv_nsec - a->tv_nsec) / 1e9;
}

int main(void) {
    static uint32_t primes[LIMIT / 2]; // pi(1e6) = 78498, so this is ample
    size_t count;
    struct timespec t0, t1;

    // Time the computation only; printing is done afterwards.
    timespec_get(&t0, TIME_UTC);
    count = sieve_primes(primes);
    timespec_get(&t1, TIME_UTC);

    for (size_t i = 0; i < count; i++)
        printf("%" PRIu32 "\n", primes[i]);

    fprintf(stderr, "Found %zu primes from 0 to %u in %.3f ms\n",
            count, LIMIT, elapsed(&t0, &t1) * 1000.0);
    return 0;
}
