# Description

🌐 **English** · [Dansk](Description-da) · [Bahasa Melayu](Description-ms)

## Purpose

The program shows how fast a CPU is. How fast it looks depends on how the
program is written, so the number is a measure of the CPU and the code
together, not of the CPU alone.

## What it does

It finds every prime from 0 to 1,000,000 in three ways, in three separate
programs so the code is easy to compare:

- `src/prime_scalar.c` builds `prime_scalar`. It tests every number with plain
  trial division by 6k ± 1, one divisor per step, with no AVX2.
- `src/prime_avx2.c` builds `prime_avx2`. It tests every number with
  `is_prime_avx2`, the same trial division with AVX2 (four divisors per step).
  See [How AVX2 works](How-AVX2-Works).
- `src/prime_sieve.c` builds `prime_sieve`. It uses a sieve of Eratosthenes
  over odd numbers, kept as a bit array (about 62 KB, so it stays in the CPU
  cache). See [How the sieve works](How-Sieve-Works).

Each prints every prime on its own line and writes a summary line to standard
error:

```text
prime_scalar: Found 78498 primes from 0 to 1000000 in 25.330 ms (average of 10 runs)
prime_avx2: Found 78498 primes from 0 to 1000000 in 12.856 ms (average of 10 runs)
prime_sieve: Found 78498 primes from 0 to 1000000 in 1.103 ms (average of 10 runs)
```

The same algorithms are also written in Rust (`src/rust/`) and C# (`src/csharp/`) for
comparison; see [Results](Results) for their times, build times and file sizes.

The times are the average of 10 runs under WSL (see [Results](Results)). AVX2
makes the trial division about 2 times faster than the scalar version, and the
sieve is about 12 times faster again, because it avoids testing each number on
its own. All three print the same primes.

## Why the time depends on the code

The same CPU gives very different times for different programs:

- the algorithm (trial division, a sieve, a segmented sieve),
- the instruction set used (scalar code, AVX2, AVX-512),
- compiler and flags (`-O3 -mavx2`, MSVC `/O2 /arch:AVX2`),
- what is measured (only the search, or also printing to a console).

Here only the search is timed. Output is collected first and printed
afterwards, so console speed does not change the number.
