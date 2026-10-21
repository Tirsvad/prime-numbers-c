# How the sieve works

🌐 **English** · [Dansk](How-Sieve-Works-da) · [Bahasa Melayu](How-Sieve-Works-ms)

`prime_sieve` (`src/prime_sieve.c`) does not test numbers one by one. It lists
all numbers up front and crosses out the ones that cannot be prime. What is
left is the primes. This is the sieve of Eratosthenes.

## The idea

1. Write down the numbers from 2 up to the limit.
2. Take the smallest number that is not crossed out. It is prime.
3. Cross out its multiples. They have it as a factor, so they are not prime.
4. Repeat from step 2 with the next number that is not crossed out.

Two facts let the program do much less work:

- **Start at p × p.** A multiple of `p` below `p × p`, such as `3p` or `5p`,
  also has a smaller prime factor, so an earlier prime has already crossed it
  out. The first new one is `p × p`.
- **Stop at sqrt(limit).** Every composite number up to the limit has a prime
  factor of at most its square root, so primes above sqrt(1,000,000) = 1,000
  have nothing left to cross out.

## A small example: primes up to 50

Only odd numbers are stored (see below), so start with 3, 5, 7, ..., 49:

| Prime p | Starts at p × p | Crossed out |
| --- | --- | --- |
| 3 | 9 | 9, 15, 21, 27, 33, 39, 45 |
| 5 | 25 | 25, 35, 45 |
| 7 | 49 | 49 |
| 9 | 81, above 50 | stop: 9 × 9 is past the limit |

Odd multiples of `p` are `2p` apart (`p × p`, `p × p + 2p`, ...), so that is the
step. What is left is 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47. With 2
added by hand, those are the 15 primes up to 50.

## Only odd numbers, one bit each

Two choices keep the data small, so it stays in the CPU cache:

- **Odd numbers only.** Every even number above 2 is composite, so the program
  skips them and adds 2 by hand. Bit `i` stands for the odd number `2i + 1`, so
  bit 0 is 1, bit 1 is 3, bit 2 is 5, and so on.
- **One bit per number.** A set bit means "composite". The bits are packed 64 to
  a `uint64_t`. Up to 1,000,000 that is 500,001 bits, about 7,800 words or
  62 KB. A plain one-byte-per-number array would be 1 MB.

To reach bit `i`, the word is `i >> 6` (that is `i / 64`) and the bit inside it
is `i & 63` (that is `i % 64`).

## The code, step by step

```c
#define SIEVE_BITS (LIMIT / 2 + 1)
#define SIEVE_WORDS ((SIEVE_BITS + 63) / 64)
```

The number of bits, and the number of 64-bit words that hold them (rounded up).

```c
static uint64_t composite[SIEVE_WORDS];
memset(composite, 0, sizeof composite);
```

Every bit starts at 0: nothing is crossed out yet.

```c
for (uint64_t p = 3; p * p <= LIMIT; p += 2) {
```

Try each odd `p` from 3, and stop when `p × p` passes the limit. `p × p` is
computed in 64 bits so it cannot overflow.

```c
    if (composite[(p >> 1) >> 6] >> ((p >> 1) & 63) & 1) continue;
```

The bit of `p` is `p >> 1` (since `p = 2i + 1`, `i = p >> 1`). If it is set,
`p` was crossed out by a smaller prime, so it is not prime and is skipped.

```c
    for (uint64_t m = (p * p) >> 1; m < SIEVE_BITS; m += p)
        composite[m >> 6] |= 1ull << (m & 63);
```

Cross out the odd multiples of `p`. The first is `p × p`, whose bit is
`(p × p) >> 1`. The next odd multiple is `2p` higher in value, which is `p`
higher in bit index, so `m += p`. `|=` sets the bit and leaves the others in the
word alone.

```c
if (LIMIT >= 2) out[count++] = 2;
for (uint32_t i = 1; i < SIEVE_BITS; i++) {
    uint32_t n = 2 * i + 1;
    if (n > LIMIT) break;
    if (!(composite[i >> 6] >> (i & 63) & 1)) out[count++] = n;
}
```

Collect the result: 2 first, then every odd `n = 2i + 1` whose bit is still 0.
The last bit can be one number past the limit, so the loop stops at `n > LIMIT`.

## Why it is faster

`prime_avx2` and `prime_scalar` test each number on its own: a prime near
1,000,000 needs about 167 loop steps (the divisors 6k ± 1 up to 1,000). The sieve
never divides. Each composite is crossed out by a few simple bit writes, fewer
than a million in total for 1,000,000 numbers, and the 62 KB array fits in the
CPU cache. That is why it is about 12 times faster than the AVX2 version and
about 23 times faster than the scalar one (see [Results](Results)).

The sieve only fits a whole range. To check one number, `is_prime_avx2` is the
better tool, and a larger limit means a larger array.
