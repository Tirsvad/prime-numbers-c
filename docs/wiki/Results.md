# Results

🌐 **English** · [Dansk](Results-da) · [Bahasa Melayu](Results-ms)

Time to find all primes from 0 to 1,000,000. Only the search is timed.

Each time is the average of 10 runs.

| CPU | Program | Result |
| --- | --- | --- |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar` | Found 78498 primes from 0 to 1000000 in 25.330 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2` | Found 78498 primes from 0 to 1000000 in 12.856 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_sieve` | Found 78498 primes from 0 to 1000000 in 1.103 ms |

AVX2 is about 2 times faster than the scalar version, and the sieve is about 12
times faster than AVX2 (about 23 times faster than scalar).

## Other languages

The same three algorithms are also written in Rust (`src/rust/`) and C#
(`src/csharp/`). They are kept as documentation, so the code of the three
languages can be read side by side. Each prints the same 78,498 primes as the C
programs. Run time, average of 10 runs under WSL on the same CPU:

| Language | `scalar` | `avx2` | `sieve` |
| --- | --- | --- | --- |
| C (gcc 12.2) | 25.330 ms | 12.856 ms | 1.103 ms |
| Rust 1.99.1 | 30.580 ms | 16.358 ms | 1.439 ms |
| C# (.NET 10) | 51.949 ms | 18.243 ms | 1.911 ms |

The order is the same in all three languages: the sieve is fastest by far, then
AVX2, then scalar. C# is slower in the scalar program and a little slower in
the sieve. The differences between C and Rust are small, and repeating the
whole measurement on another occasion moved averages by up to about 20% (for
example C scalar between 25 and 29 ms), so do not read a difference of that size
as a real one. Programs differ in more than the language (compiler, runtime and
library details), so read the table as a rough comparison.

- **Rust** 1.99.1, `cargo build --release` (`opt-level = 3`). The AVX2 version
  uses `std::arch` intrinsics with `#[target_feature(enable = "avx2")]`.
- **C#** on .NET 10, a Release build with tiered compilation off, so the JIT
  produces fully optimised code at once. The AVX2 version uses
  `System.Runtime.Intrinsics`. The JIT time of the timed code is part of the
  measured time, and the .NET runtime start-up is not.

### Why the divide width matters for the scalar program

The scalar program spends its time in the `n % d` division. In a first version
of the C program `d` was a `uint64_t`, and gcc turned `n % d` into a 64-bit
divide (`divq`). That version took about 46.7 ms, while the Rust program took
about 27 ms in the same comparison. The reason is that the Rust compiler (LLVM)
saw that `n` and `d` fit in 32 bits and used the cheaper 32-bit divide (`divl`);
on this CPU a 64-bit divide is slower than a 32-bit one.

`prime_scalar.c` now declares `d` as `uint32_t` (only `(uint64_t)d * d` is
computed in 64 bits, so the bound cannot overflow). C is then as fast as Rust.
The table shows this version.

## Build time and file size

Build time is the average of 5 clean builds (`rm -rf` of the build output
before each) under WSL. File size is the size of the file that is built.

| | Build time | File size | Stripped |
| --- | --- | --- | --- |
| C `prime_scalar` (gcc `-O3`) | 207 ms | 16,136 bytes | 14,496 bytes |
| C `prime_avx2` (gcc `-O3 -mavx2`) | 416 ms | 16,216 bytes | 14,504 bytes |
| C `prime_sieve` (gcc `-O3 -mavx2`) | 237 ms | 16,224 bytes | 14,504 bytes |
| C all three with `make` | 1,144 ms | 48,576 bytes | |
| Rust `cargo build --release` (all three) | 397 ms | 491,824 bytes | 376,288 bytes |
| C# `dotnet build -c Release` (all three) | 990 ms | `prime.dll` 7,680 bytes | |

- **One program or three.** C builds three executables. The Rust and C# programs
  contain all three algorithms and pick one from the first argument
  (`prime scalar|avx2|sieve`), so one build covers all three. The "all three"
  rows are the ones to compare.
- **Rust is larger** because the standard library is linked into the program
  (about 376 KB after `strip`). The C programs use the system C library, which
  is not part of their size.
- **C#** builds a small `prime.dll` (7,680 bytes; 102,847 bytes for the whole
  output folder with `prime.pdb` and the `.json` files). It does not contain the
  .NET runtime, which must be installed to run it. The first C# build was the
  slowest (1,732 ms against about 800 ms for the other four), because the build
  server starts then.
- **Build time includes** compiling a program that has no other dependencies.
  A larger program, an incremental build or a cold cache would give other
  numbers.

## How the results were measured

The C programs were built with `make` and the Rust and C# programs with the
commands in [Build and run](Build-and-Run), and everything was run under WSL 2 (Windows Subsystem for
Linux) on Windows 11: Debian with gcc 12.2 and Linux kernel 6.18. WSL runs a
real Linux kernel in a lightweight virtual machine on the same 13th Gen
Intel(R) Core(TM) i5-13600K, so the CPU is the same as on the host. Compiler
flags: `-O3 -mavx2` for `prime_avx2` and `prime_sieve`, and `-O3` without
`-mavx2` for `prime_scalar`, so the compiler cannot use AVX2 there. Each
program was started 10 times and the tables show the average. Run-to-run
times vary by a few milliseconds, and every run is kept in the average.

Timings under WSL can differ a little from native Linux or Windows, so
compare numbers only when they were measured the same way.

A result is only comparable with another when the code, compiler and flags
are the same. To add a result, run the programs on your machine (see
[Build and run](Build-and-Run)) and add a row per program with your CPU name
and the summary line.
