# Cara AVX2 berfungsi

🌐 [English](How-AVX2-Works) · [Dansk](How-AVX2-Works-da) · **Bahasa Melayu**

AVX2 ialah set arahan CPU yang melakukan operasi yang sama pada beberapa nombor
serentak (SIMD: satu arahan, banyak data). `is_prime_avx2` menggunakannya untuk
menguji **empat pembahagi setiap langkah** dan bukan satu.

## Idea

Nombor `n` ialah nombor perdana jika tiada nombor dari 2 hingga sqrt(n) yang
membahaginya. Dua langkah mengurangkan kerja:

1. **Langkau kebanyakan calon.** Setiap nombor perdana melebihi 3 berbentuk
   6k - 1 atau 6k + 1 (5, 7, 11, 13, 17, 19, 23, 25, ...). Nombor genap dan
   gandaan 3 ditolak sebelum gelung, jadi hanya pembahagi ini dicuba.
2. **Uji empat sekali gus.** Satu daftar AVX2 256-bit memuatkan empat nilai
   `double`, jadi satu pembahagian, satu pendaraban dan satu perbandingan
   mengendalikan empat pembahagi.

Setiap pusingan gelung menguji pembahagi `d, d+2, d+6, d+8` kemudian maju 12:
`5,7,11,13` kemudian `17,19,23,25` kemudian `29,31,35,37`, dan seterusnya.

## Kod, langkah demi langkah

Komen dalam kod ditulis dalam bahasa Inggeris, seperti kod itu sendiri.

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

## Mengapa topeng `in_range` diperlukan

Kumpulan terakhir boleh mengandungi pembahagi yang melebihi sqrt(n). Tanpa
topeng, satu lorong boleh melaporkan "faktor" yang bukan faktor. Untuk `n = 25`
had ialah 5, jadi hanya lorong 0 (pembahagi 5) dikira dan 25 ditolak dengan
betul. Untuk `n = 29` tiada yang membahagikannya dan 29 ialah nombor perdana.

## Mengapa `double` dan bukan `float` atau integer

AVX2 tiada pembahagian integer, jadi pembahagian dibuat dalam titik terapung.
`double` memuatkan setiap integer 32-bit dengan tepat dan pembahagian IEEE
dibundarkan dengan betul, jadi `floor(n / d)` tepat untuk `n` sehingga
`UINT32_MAX`. `float` 32-bit hanya mempunyai ketepatan 24 bit dan akan memberi
hasil bahagi yang salah. Ini juga sebabnya fungsi ini hanya menyokong `n`
sehingga 4,294,967,295.

## Intrinsic yang digunakan

| Intrinsic | Fungsinya |
| --- | --- |
| `_mm256_set1_pd` | salin satu double ke semua 4 lorong |
| `_mm_setr_epi32` | bina 4 nilai int32 mengikut urutan |
| `_mm256_cvtepi32_pd` | tukar 4 int32 kepada 4 double |
| `_mm256_div_pd` / `_mm256_mul_pd` / `_mm256_sub_pd` | bahagi, darab, tolak dalam 4 lorong |
| `_mm256_floor_pd` | bundarkan 4 double ke bawah |
| `_mm256_cmp_pd` | banding 4 lorong, beri topeng semua-satu atau semua-sifar setiap lorong |
| `_mm256_and_pd` | DAN bitwise bagi dua topeng |
| `_mm256_movemask_pd` | gabungkan 4 topeng lorong menjadi integer 4-bit |
| `_mm_add_epi32` | tambah 4 nilai int32 |

Ia datang daripada `<immintrin.h>`. Bina dengan `-mavx2` (gcc/clang) atau
`/arch:AVX2` (MSVC). Pada CPU tanpa AVX2 program akan ranap dengan arahan tidak
sah.
