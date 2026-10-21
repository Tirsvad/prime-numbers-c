#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <omp.h> // OpenMP: parallel for

// Returns true if n is prime, using plain scalar trial division by 6k +/- 1.
// Same idea as is_prime_avx2 in prime_avx2.c, but one divisor per step and no
// AVX2: it uses integer division and needs no special instruction set.
static bool is_prime_scalar(uint32_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    // Every prime above 3 is 6k - 1 or 6k + 1, so test d and d + 2, then move 6
    // forward. Stop once d * d > n: a factor above sqrt(n) pairs with one below it.
    // d is 32-bit so n % d is a cheap 32-bit divide; only d * d is computed in
    // 64 bits, so it cannot overflow for n near UINT32_MAX.
    for (uint32_t d = 5; (uint64_t)d * d <= n; d += 6)
        if (n % d == 0 || n % (d + 2) == 0)
            return false;

    // No divisor found up to sqrt(n): n is prime.
    return true;
}

#define LIMIT 1000000u

// Seconds elapsed between two timespec_get(TIME_UTC) samples.
static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (double)(b->tv_sec - a->tv_sec) + (double)(b->tv_nsec - a->tv_nsec) / 1e9;
}

int main(void) {
    static uint8_t is_p[LIMIT + 1];    // is_p[n] = 1 when n is prime
    static uint32_t primes[LIMIT / 2]; // pi(1e6) = 78498, so this is ample
    size_t count = 0;
    struct timespec t0, t1;

    // Time the computation only; printing is done afterwards.
    timespec_get(&t0, TIME_UTC);

    // The numbers are independent, so the loop is split across threads. Each
    // thread takes the next block of 1024 numbers when it is free (dynamic),
    // because larger numbers cost more to test. Every n writes its own byte, so
    // the threads never touch the same memory.
    #pragma omp parallel for schedule(dynamic, 1024)
    for (uint32_t n = 0; n <= LIMIT; n++)
        is_p[n] = is_prime_scalar(n);

    // Collect the primes in order on one thread, so the output is sorted.
    for (uint32_t n = 0; n <= LIMIT; n++)
        if (is_p[n]) primes[count++] = n;
    timespec_get(&t1, TIME_UTC);

    for (size_t i = 0; i < count; i++)
        printf("%" PRIu32 "\n", primes[i]);

    fprintf(stderr, "[%d threads] Found %zu primes from 0 to %u in %.3f ms\n",
            omp_get_max_threads(), count, LIMIT, elapsed(&t0, &t1) * 1000.0);
    return 0;
}
