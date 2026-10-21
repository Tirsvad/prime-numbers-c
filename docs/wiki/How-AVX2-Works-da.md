# Sådan virker AVX2

🌐 [English](How-AVX2-Works) · **Dansk** · [Bahasa Melayu](How-AVX2-Works-ms)

AVX2 er et sæt CPU-instruktioner, der udfører den samme operation på flere tal
på én gang (SIMD: én instruktion, flere data). `is_prime_avx2` bruger det til at
teste **fire divisorer pr. trin** i stedet for én.

## Idéen

Et tal `n` er et primtal, hvis intet tal fra 2 til sqrt(n) går op i det. To
trin mindsker arbejdet:

1. **Spring de fleste kandidater over.** Hvert primtal over 3 har formen 6k - 1
   eller 6k + 1 (5, 7, 11, 13, 17, 19, 23, 25, ...). Lige tal og multipla af 3
   afvises før løkken, så kun disse divisorer prøves.
2. **Test fire ad gangen.** Ét 256-bit AVX2-register rummer fire `double`-værdier,
   så én division, én multiplikation og én sammenligning klarer fire divisorer.

Hver gennemgang af løkken tester divisorerne `d, d+2, d+6, d+8` og går derefter
12 frem: `5,7,11,13` så `17,19,23,25` så `29,31,35,37` og så videre.

## Koden, trin for trin

Kommentarerne i koden er på engelsk, ligesom koden selv.

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

## Hvorfor `in_range`-masken er nødvendig

Den sidste gruppe kan indeholde divisorer over sqrt(n). Uden masken kunne en
bane melde en "faktor", der ikke er en. For `n = 25` er grænsen 5, så kun bane 0
(divisor 5) tæller, og 25 afvises korrekt. For `n = 29` går intet op i tallet,
og 29 er et primtal.

## Hvorfor `double` og ikke `float` eller heltal

AVX2 har ingen heltalsdivision, så divisionen foregår i flydende tal. En
`double` rummer ethvert 32-bit heltal præcist, og IEEE-division er korrekt
afrundet, så `floor(n / d)` er præcis for `n` op til `UINT32_MAX`. En 32-bit
`float` har kun 24 bits præcision og ville give forkerte kvotienter. Det er også
grunden til, at funktionen kun understøtter `n` op til 4.294.967.295.

## Brugte intrinsics

| Intrinsic | Hvad den gør |
| --- | --- |
| `_mm256_set1_pd` | kopierer én double til alle 4 baner |
| `_mm_setr_epi32` | bygger 4 int32-værdier i rækkefølge |
| `_mm256_cvtepi32_pd` | konverterer 4 int32 til 4 double |
| `_mm256_div_pd` / `_mm256_mul_pd` / `_mm256_sub_pd` | dividerer, multiplicerer, trækker fra i 4 baner |
| `_mm256_floor_pd` | runder 4 doubles ned |
| `_mm256_cmp_pd` | sammenligner 4 baner og giver en maske med enere eller nuller pr. bane |
| `_mm256_and_pd` | bitvis OG af to masker |
| `_mm256_movemask_pd` | samler de 4 banemasker til et 4-bit heltal |
| `_mm_add_epi32` | lægger 4 int32-værdier sammen |

De kommer fra `<immintrin.h>`. Byg med `-mavx2` (gcc/clang) eller `/arch:AVX2`
(MSVC). På en CPU uden AVX2 ville programmet gå ned med en ugyldig instruktion.

## Sammenlign med de andre programmer

Den samme prøvedivision uden AVX2 findes i `src/prime_scalar.c`, og sien findes
i `src/prime_sieve.c`, så de tre kan læses side om side. Se
[Resultater](Results-da) for, hvor meget hurtigere hver af dem er.
