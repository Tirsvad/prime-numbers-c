# Results

🌐 **English** · [Dansk](Results-da) · [Bahasa Melayu](Results-ms)

Time to find all primes from 0 to 1,000,000. Only the search is timed.

Each time is the average of 10 runs.

| CPU | Program | Threads | Result |
| --- | --- | --- | --- |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar` | 1 thread | Found 78498 primes from 0 to 1000000 in 27.675 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar_mt` | 20 threads | Found 78498 primes from 0 to 1000000 in 4.790 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2` | 1 thread | Found 78498 primes from 0 to 1000000 in 13.804 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2_mt` | 20 threads | Found 78498 primes from 0 to 1000000 in 3.814 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_sieve` | 1 thread | Found 78498 primes from 0 to 1000000 in 1.183 ms |

AVX2 is about 2 times faster than the scalar version. The `_mt` programs run on
all 20 threads of the CPU (see below). The sieve is the fastest of all, even
faster than the 20-thread trial division, because it does far less work.

## Parallel versions

`prime_scalar_mt` and `prime_avx2_mt` do the same trial division as
`prime_scalar` and `prime_avx2`, but on all CPU threads with OpenMP. Every
number is tested on its own, so the loop can be split: each thread takes the
next block of 1,024 numbers when it is free (`schedule(dynamic, 1024)`),
because larger numbers cost more to test. Each number writes its own byte in a
flag array, so the threads never touch the same memory, and the primes are
collected in order on one thread afterwards, so the output is sorted. The time
includes that last step. The sieve was not made parallel: it takes only about
1 ms, which is about the cost of starting the threads.

`prime_avx2_mt` is about 3.6 times faster than `prime_avx2`, and
`prime_scalar_mt` about 5.8 times faster than `prime_scalar`, on 20 threads.
That is far from 20 times. How the time falls with the number of threads
(`OMP_NUM_THREADS`, passive wait, average of 10 runs):

| Threads | `prime_scalar_mt` | `prime_avx2_mt` |
| --- | --- | --- |
| 1 | 28.916 ms | 15.069 ms |
| 2 | 16.637 ms | 8.969 ms |
| 4 | 8.251 ms | 4.808 ms |
| 8 | 5.746 ms | 4.014 ms |
| 14 | 4.960 ms | 3.602 ms |
| 20 | 4.910 ms | 3.490 ms |

- The time stops falling after about 4 to 8 threads. Part of the work is not
  parallel (starting the threads, collecting the primes in order), and this CPU
  has 6 fast cores with 2 threads each and 8 slower cores, so the last threads
  add little.
- **Wait policy.** By default OpenMP threads that run out of work spin and wait.
  With all 20 threads busy under WSL this made the programs slow and unstable
  (between 10 and 20 ms in three measurements, against 3.5 to 4.9 ms). The
  table above and the main table use `OMP_WAIT_POLICY=passive`, where waiting
  threads sleep. If the `_mt` programs are slower than the serial ones on your
  machine, try setting it:

```bash
OMP_WAIT_POLICY=passive ./dist/prime_avx2_mt
```

## Background thread and GPU

Two more programs, measured in the same session as the tables above, next to
the serial and `_mt` C programs for comparison:

| Program | Average of 10 runs |
| --- | --- |
| `prime_scalar` | 27.675 ms |
| `prime_avx2` | 13.804 ms |
| `prime_sieve` | 1.183 ms |
| `prime_sieve_thread` (background thread) | 1.268 ms |
| `prime_scalar_mt` (20 threads, passive wait) | 4.790 ms |
| `prime_avx2_mt` (20 threads, passive wait) | 3.814 ms |
| `prime_cuda` (RTX 3050) | 5.495 ms |

### Background thread sieve (POSIX)

`prime_sieve_thread` (`src/prime_sieve_thread.c`) runs the same sieve as
`prime_sieve`, but on a background thread: the main thread starts it with
`pthread_create`, is free to do other work, and here simply waits for the result
with `pthread_join`. One extra thread does the same work as the serial sieve,
so it cannot be faster; starting and joining the thread costs about 0.09 ms
(1.268 ms against 1.183 ms). The point of this shape is to keep a program
responsive while the work runs, not to speed the work up. It needs POSIX
threads, so it is built with `-pthread`.

### GPU (CUDA)

`prime_cuda` (`src/prime_cuda.cu`) runs the trial division on an NVIDIA GPU:
one GPU thread tests one number, writes a flag byte, and the CPU collects the
primes in order. It is built with `nvcc` (`make cuda`) and is not part of
`make`. The GPU here is an NVIDIA GeForce RTX 3050, reached from WSL through the
Windows driver, with CUDA 13.3.

- The time in the table, 5.5 ms, includes allocating GPU memory, running the
  kernel, copying the flags back and collecting the primes. The kernel itself
  takes about 2.5 ms.
- It does **not** include the one-time start-up of the CUDA runtime, which
  took 130 to 170 ms in these runs (first CUDA call). The program prints it
  separately.
- The GPU is slower than the 20-thread CPU programs (3.8 to 4.8 ms) and than
  the sieve (1.2 ms) at this size. There is little work (1,000,000 numbers) for
  thousands of GPU threads, and the copy and start-up costs are fixed. The
  kernel is probably also held back because 32 neighbouring numbers run in
  lockstep and a group is as slow as its slowest number: a prime needs about 160
  loop steps while most composites stop at once. This was not measured. A much
  larger limit would favour the GPU, but that was not tried.

## Other languages

The same algorithms are also written in Rust (`src/rust/`) and C#
(`src/csharp/`). They are kept as documentation, so the code of the three
languages can be read side by side. Each prints the same 78,498 primes as the C
programs. Run time, average of 10 runs under WSL on the same CPU; `-mt` is the
parallel version on all 20 threads:

| Language | `scalar` | `scalar-mt` | `avx2` | `avx2-mt` | `sieve` |
| --- | --- | --- | --- | --- | --- |
| C (gcc 12.2) | 27.675 ms | 4.790 ms | 13.804 ms | 3.814 ms | 1.183 ms |
| Rust 1.99.1 | 29.020 ms | 5.413 ms | 14.580 ms | 4.415 ms | 1.294 ms |
| C# (.NET 10) | 48.883 ms | 18.855 ms | 16.704 ms | 17.894 ms | 1.850 ms |

The order is the same in all three languages: the sieve is fastest by far, and
AVX2 is faster than scalar. C and Rust are close; repeating the whole
measurement on another occasion moved averages by up to about 20% (for example
C scalar between 25 and 29 ms), so do not read a difference of that size as a
real one. Programs differ in more than the language (compiler, runtime and
library details), so read the table as a rough comparison.

- **Rust** 1.99.1, `cargo build --release` (`opt-level = 3`). The AVX2 version
  uses `std::arch` intrinsics with `#[target_feature(enable = "avx2")]`. The
  `-mt` versions use `std::thread::scope` with a shared block counter and no
  extra crates.
- **C#** on .NET 10, a Release build with tiered compilation off, so the JIT
  produces fully optimised code at once. The AVX2 version uses
  `System.Runtime.Intrinsics`. The `-mt` versions use `Parallel.For`. The JIT
  time of the timed code is part of the measured time, and the .NET runtime
  start-up is not.
- **C# is slow in the `-mt` modes** (about 18 ms, no better than serial AVX2).
  The first `Parallel.For` in a process pays a start-up cost of about 14 ms
  (thread pool and library start-up). Running the same call again in the same
  process took 3 to 4 ms, as fast as Rust, and so did a loop of plain threads
  started by hand. This was checked in a throwaway experiment. A program that
  does only one short parallel job, as here, pays that cost every time.

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
| C `prime_scalar_mt` (`-O3 -fopenmp`) | 189 ms | 16,720 bytes | 14,528 bytes |
| C `prime_avx2` (`-O3 -mavx2`) | 416 ms | 16,216 bytes | 14,504 bytes |
| C `prime_avx2_mt` (`-O3 -mavx2 -fopenmp`) | 396 ms | 16,808 bytes | 14,536 bytes |
| C `prime_sieve` (`-O3 -mavx2`) | 237 ms | 16,224 bytes | 14,504 bytes |
| C all five (sum of the five rows) | 1,445 ms | 82,104 bytes | |
| C `prime_sieve_thread` (`-O3 -pthread`) | 206 ms | 16,432 bytes | 14,528 bytes |
| C `prime_cuda` (`nvcc -O3 -arch=native`) | 820 ms | 1,087,000 bytes | 769,904 bytes |
| Rust `cargo build --release` (all five) | 521 ms | 563,504 bytes | 428,184 bytes |
| C# `dotnet build -c Release` (all five) | 1,069 ms | `prime.dll` 8,704 bytes | |

- **`prime_cuda` is larger** (about 1 MB) because the CUDA runtime is linked in
  statically, and it holds the GPU code for this machine's GPU. It needs the NVIDIA
  driver to run.
- **One program or five.** C builds five executables. The Rust and C# programs
  contain all five modes and pick one from the first argument
  (`prime scalar|scalar-mt|avx2|avx2-mt|sieve`), so one build covers all of
  them. The "all five" rows are the ones to compare.
- **Rust is larger** because the standard library is linked into the program
  (about 428 KB after `strip`). The C programs use the system C library, which
  is not part of their size. The `-fopenmp` programs link the OpenMP library
  (`libgomp`) dynamically, so it is not part of their size either.
- **C#** builds a small `prime.dll` (8,704 bytes; 104,339 bytes for the whole
  output folder with `prime.pdb` and the `.json` files). It does not contain the
  .NET runtime, which must be installed to run it. The first C# build was the
  slowest (1,974 ms against about 850 ms for the other four), because the build
  server starts then.
- **Build time includes** compiling a program that has no other dependencies.
  A larger program, an incremental build or a cold cache would give other
  numbers.

## How the results were measured

The C programs were built with `make` and the Rust and C# programs with the
commands in [Build and run](Build-and-Run), and everything was run under WSL 2
(Windows Subsystem for Linux) on Windows 11: Debian with gcc 12.2 and Linux
kernel 6.18. WSL runs a real Linux kernel in a lightweight virtual machine on
the same 13th Gen Intel(R) Core(TM) i5-13600K (20 threads), so the CPU is the
same as on the host. Compiler flags: `-O3 -mavx2` for `prime_avx2` and
`prime_sieve`, `-O3` without `-mavx2` for `prime_scalar` so the compiler cannot
use AVX2 there, and `-fopenmp` for the `_mt` programs. Each program was started
10 times and the tables show the average. Run-to-run times vary by a few
milliseconds, and every run is kept in the average.

Timings under WSL can differ a little from native Linux or Windows, so
compare numbers only when they were measured the same way.

A result is only comparable with another when the code, compiler and flags
are the same. To add a result, run the programs on your machine (see
[Build and run](Build-and-Run)) and add a row per program with your CPU name,
the number of threads and the summary line.
