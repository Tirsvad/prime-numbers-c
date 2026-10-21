# Results

🌐 **English** · [Dansk](Results-da) · [Bahasa Melayu](Results-ms)

Time to find all primes from 0 to 1,000,000. Only the search is timed.

Each time is the average of 10 runs.

| CPU | Program | Result |
| --- | --- | --- |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar` | Found 78498 primes from 0 to 1000000 in 46.692 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2` | Found 78498 primes from 0 to 1000000 in 15.841 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_sieve` | Found 78498 primes from 0 to 1000000 in 1.191 ms |

AVX2 is about 3 times faster than the scalar version, and the sieve is about 13
times faster than AVX2 (about 39 times faster than scalar).

## How the results were measured

The programs were built with `make` and run under WSL 2 (Windows Subsystem for
Linux) on Windows 11: Debian with gcc 12.2 and Linux kernel 6.18. WSL runs a
real Linux kernel in a lightweight virtual machine on the same 13th Gen
Intel(R) Core(TM) i5-13600K, so the CPU is the same as on the host. Compiler
flags: `-O3 -mavx2` for `prime_avx2` and `prime_sieve`, and `-O3` without
`-mavx2` for `prime_scalar`, so the compiler cannot use AVX2 there. Each
program was started 10 times and the table shows the average. Run-to-run
times vary by a few milliseconds, and every run is kept in the average.

Timings under WSL can differ a little from native Linux or Windows, so
compare numbers only when they were measured the same way.

A result is only comparable with another when the code, compiler and flags
are the same. To add a result, run the programs on your machine (see
[Build and run](Build-and-Run)) and add a row per program with your CPU name
and the summary line.
