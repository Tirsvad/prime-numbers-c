# Description

🌐 **English** · [Dansk](Description-da) · [Bahasa Melayu](Description-ms)

## Purpose

The program shows how fast a CPU is. How fast it looks depends on how the
program is written, so the number is a measure of the CPU and the code
together, not of the CPU alone.

## What it does

It tests every number from 0 to 1,000,000 with `is_prime_avx2`, which uses
AVX2 trial division by 6k ± 1 (four divisors per step). It prints each prime
on its own line and writes a summary line to standard error:

```text
Found 78498 primes from 0 to 1000000 in 13.806 ms
```

78,498 is the known count of primes up to 1,000,000, so the summary line also
works as a correctness check.

## Why the time depends on the code

The same CPU gives very different times for different programs:

- the algorithm (trial division, a sieve, a segmented sieve),
- the instruction set used (scalar code, AVX2, AVX-512),
- compiler and flags (`-O3 -mavx2`, MSVC `/O2 /arch:AVX2`),
- what is measured (only the search, or also printing to a console).

Here only the search is timed. Output is collected first and printed
afterwards, so console speed does not change the number.
