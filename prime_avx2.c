#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
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

int main(void) {
    // Known large prime: 1,000,000,007
    uint64_t num = 1000000007;

    printf("Checking if %" PRIu64 " is prime using AVX2...\n", num);

    if (is_prime_avx2(num))
        printf("%" PRIu64 " is a PRIME number.\n", num);
    else
        printf("%" PRIu64 " is a COMPOSITE number.\n", num);

    return 0;
}
