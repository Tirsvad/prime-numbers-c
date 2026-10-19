#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
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

    const double limit = floor(sqrt((double)n));

    __m256d v_n     = _mm256_set1_pd((double)n);
    __m256d v_limit = _mm256_set1_pd(limit);

    // Divisors 5, 7, 11, 13 (6k +/- 1); each iteration advances by 12.
    __m128i v_div_i = _mm_setr_epi32(5, 7, 11, 13);
    const __m128i v_step = _mm_set1_epi32(12);

    for (double base = 5.0; base <= limit; base += 12.0) {
        __m256d v_d   = _mm256_cvtepi32_pd(v_div_i);
        __m256d v_q   = _mm256_floor_pd(_mm256_div_pd(v_n, v_d));
        __m256d v_rem = _mm256_sub_pd(v_n, _mm256_mul_pd(v_q, v_d));

        __m256d is_zero = _mm256_cmp_pd(v_rem, _mm256_setzero_pd(), _CMP_EQ_OQ);
        __m256d in_range = _mm256_cmp_pd(v_d, v_limit, _CMP_LE_OQ);

        // A lane only counts if it divides n and the divisor is <= sqrt(n)
        if (_mm256_movemask_pd(_mm256_and_pd(is_zero, in_range)) != 0)
            return false;

        v_div_i = _mm_add_epi32(v_div_i, v_step);
    }

    return true;
}

#define LIMIT 1000000u

// Seconds elapsed between two timespec_get(TIME_UTC) samples.
static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (double)(b->tv_sec - a->tv_sec) + (double)(b->tv_nsec - a->tv_nsec) / 1e9;
}

int main(void) {
    static uint32_t primes[LIMIT / 2]; // pi(1e6) = 78498, so this is ample
    size_t count = 0;
    struct timespec t0, t1;

    // Time the computation only; printing is done afterwards.
    timespec_get(&t0, TIME_UTC);
    for (uint32_t n = 0; n <= LIMIT; n++)
        if (is_prime_avx2(n)) primes[count++] = n;
    timespec_get(&t1, TIME_UTC);

    for (size_t i = 0; i < count; i++)
        printf("%" PRIu32 "\n", primes[i]);

    fprintf(stderr, "Found %zu primes from 0 to %u in %.3f ms\n",
            count, LIMIT, elapsed(&t0, &t1) * 1000.0);
    return 0;
}
