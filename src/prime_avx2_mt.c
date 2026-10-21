#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <omp.h> // OpenMP: parallel for
#include <immintrin.h> // AVX2 intrinsics

// Returns true if n is prime, using AVX2 trial division by 6k +/- 1.
// Supports n up to UINT32_MAX.
//
// Four divisors are tested per iteration in double precision. A double holds
// every 32-bit integer exactly and IEEE division is correctly rounded, so
// floor(n / d) is exact here (a 24-bit float quotient is not).
bool is_prime_avx2(uint64_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    if (n > UINT32_MAX) return false; // out of supported range

    // Only divisors up to sqrt(n) need testing: a factor above it pairs with one below it.
    const double limit = floor(sqrt((double)n));

    // Broadcast n and the limit into all 4 double lanes of a 256-bit register.
    __m256d v_n     = _mm256_set1_pd((double)n);
    __m256d v_limit = _mm256_set1_pd(limit);

    // Divisors 5, 7, 11, 13 (6k +/- 1); each iteration advances by 12.
    // Kept as 4 x int32 in a 128-bit register, converted to double each pass.
    __m128i v_div_i = _mm_setr_epi32(5, 7, 11, 13);
    const __m128i v_step = _mm_set1_epi32(12);

    // base is the first divisor of the group; stop when it passes sqrt(n).
    for (double base = 5.0; base <= limit; base += 12.0) {
        // Convert the 4 int32 divisors to 4 doubles.
        __m256d v_d   = _mm256_cvtepi32_pd(v_div_i);
        // q = floor(n / d) for all 4 divisors at once.
        __m256d v_q   = _mm256_floor_pd(_mm256_div_pd(v_n, v_d));
        // rem = n - q * d, which is n mod d.
        __m256d v_rem = _mm256_sub_pd(v_n, _mm256_mul_pd(v_q, v_d));

        // Lane mask: all bits set where the remainder is 0 (d divides n).
        __m256d is_zero = _mm256_cmp_pd(v_rem, _mm256_setzero_pd(), _CMP_EQ_OQ);
        // Lane mask: all bits set where the divisor is <= sqrt(n).
        __m256d in_range = _mm256_cmp_pd(v_d, v_limit, _CMP_LE_OQ);

        // A lane only counts if it divides n and the divisor is <= sqrt(n).
        // movemask packs the sign bit of each lane into 4 bits; non-zero = a hit.
        if (_mm256_movemask_pd(_mm256_and_pd(is_zero, in_range)) != 0)
            return false;

        // Move to the next four divisors: 5,7,11,13 -> 17,19,23,25 -> ...
        v_div_i = _mm_add_epi32(v_div_i, v_step);
    }

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
        is_p[n] = is_prime_avx2(n);

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
