# How AVX2 works

🌐 **English** · [Dansk](How-AVX2-Works-da) · [Bahasa Melayu](How-AVX2-Works-ms)

AVX2 is a set of CPU instructions that do the same operation on several
numbers at once (SIMD: single instruction, multiple data). `is_prime_avx2`
uses it to test **four divisors per step** instead of one.

## The idea

A number `n` is prime if no number from 2 to sqrt(n) divides it. Two steps
shrink the work:

1. **Skip most candidates.** Every prime above 3 has the form 6k - 1 or 6k + 1
   (5, 7, 11, 13, 17, 19, 23, 25, ...). Even numbers and multiples of 3 are
   rejected before the loop, so only these divisors are tried.
2. **Test four at a time.** One 256-bit AVX2 register holds four `double`
   values, so one divide, one multiply and one compare handle four divisors.

Each loop pass tests the divisors `d, d+2, d+6, d+8` and then moves 12 forward:
`5,7,11,13` then `17,19,23,25` then `29,31,35,37`, and so on.

## The code, step by step

```c
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
```

## Why the `in_range` mask is needed

The last group can contain divisors above sqrt(n). Without the mask such a
lane could report a "factor" that is not one. For `n = 25` the limit is 5, so
only lane 0 (divisor 5) counts and 25 is correctly rejected; for `n = 29`
nothing divides it and 29 is prime.

## Why `double` and not `float` or integers

AVX2 has no integer divide, so the division is done in floating point. A
`double` holds every 32-bit integer exactly and IEEE division is correctly
rounded, so `floor(n / d)` is exact for `n` up to `UINT32_MAX`. A 32-bit
`float` has only 24 bits of precision and would give wrong quotients. This is
also why the function supports `n` only up to 4,294,967,295.

## Intrinsics used

| Intrinsic | What it does |
| --- | --- |
| `_mm256_set1_pd` | copy one double into all 4 lanes |
| `_mm_setr_epi32` | build 4 int32 values in order |
| `_mm256_cvtepi32_pd` | convert 4 int32 to 4 double |
| `_mm256_div_pd` / `_mm256_mul_pd` / `_mm256_sub_pd` | divide, multiply, subtract in 4 lanes |
| `_mm256_floor_pd` | round 4 doubles down |
| `_mm256_cmp_pd` | compare 4 lanes, giving an all-ones or all-zeros mask per lane |
| `_mm256_and_pd` | bitwise AND of two masks |
| `_mm256_movemask_pd` | collapse the 4 lane masks into a 4-bit integer |
| `_mm_add_epi32` | add 4 int32 values |

They come from `<immintrin.h>`. Build with `-mavx2` (gcc/clang) or
`/arch:AVX2` (MSVC); on a CPU without AVX2 the program would crash with an
illegal instruction.

## Compare with the other programs

The same trial division without AVX2 is in `src/prime_scalar.c`, and the sieve
is in `src/prime_sieve.c` (see [How the sieve works](How-Sieve-Works)), so the
three can be read side by side. See [Results](Results) for how much faster each is.
