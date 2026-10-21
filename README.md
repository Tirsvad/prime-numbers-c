# prime-numbers-c

Lists every prime number from 0 to 1,000,000 and reports how long the search
took. It shows how fast a CPU is, but the result depends on how the program is
written: algorithm, instruction set (here AVX2), compiler and flags.

Several programs do the same job in different ways, so the code is easy to compare:

- `prime_scalar` tests each number with plain trial division (no AVX2).
- `prime_avx2` tests each number with AVX2 trial division, four divisors per step.
- `prime_scalar_mt` and `prime_avx2_mt` are the same two on all CPU threads (OpenMP).
- `prime_sieve` uses a sieve of Eratosthenes over odd numbers, stored as a bit array.
- `prime_sieve_thread` runs that sieve on a background POSIX thread (pthreads).
- `prime_cuda` runs the trial division on an NVIDIA GPU (CUDA); built with `make cuda`.

Example results, the average of 10 runs on a 13th Gen Intel(R) Core(TM)
i5-13600K (20 threads). They were built and run under WSL 2 (Debian, gcc 12.2,
`-O3`; `-mavx2` for the AVX2 and sieve programs, `-fopenmp` for the `_mt`
programs, run with `OMP_WAIT_POLICY=passive`), see the wiki page Results:

```text
prime_scalar: Found 78498 primes from 0 to 1000000 in 27.675 ms (average of 10 runs)
prime_scalar_mt: [20 threads] Found 78498 primes from 0 to 1000000 in 4.790 ms (average of 10 runs)
prime_avx2: Found 78498 primes from 0 to 1000000 in 13.804 ms (average of 10 runs)
prime_avx2_mt: [20 threads] Found 78498 primes from 0 to 1000000 in 3.814 ms (average of 10 runs)
prime_sieve: Found 78498 primes from 0 to 1000000 in 1.183 ms (average of 10 runs)
prime_sieve_thread: [background thread] Found 78498 primes from 0 to 1000000 in 1.268 ms (average of 10 runs)
prime_cuda: [CUDA start-up 130-170 ms not included] Found 78498 primes from 0 to 1000000 in 5.495 ms (average of 10 runs)
```

## Build and run

Needs `gcc` with OpenMP (Linux, or MinGW-w64 / MSYS2 on Windows), `make` and a CPU with AVX2.

```bash
make
```

```bash
make run
```

```bash
make clean
```

The executables are built into `dist/`. The primes go to standard output and the
timing line to standard error, so this saves the list and still shows the time
(use any of the five programs):

```bash
./dist/prime_sieve > primes.txt
```

The Rust and C# versions are kept for comparison and are not built by `make`; see
the wiki page Build and run.

## Layout

| Path | Content |
| --- | --- |
| `src/prime_scalar.c` | plain scalar trial-division program |
| `src/prime_scalar_mt.c` | the scalar program on all CPU threads (OpenMP) |
| `src/prime_avx2.c` | AVX2 trial-division program |
| `src/prime_avx2_mt.c` | the AVX2 program on all CPU threads (OpenMP) |
| `src/prime_sieve.c` | sieve of Eratosthenes program |
| `src/prime_sieve_thread.c` | the sieve on a background POSIX thread |
| `src/prime_cuda.cu` | trial division on an NVIDIA GPU (CUDA) |
| `src/rust/` | the same algorithms in Rust (documentation, Cargo project) |
| `src/csharp/` | the same algorithms in C# (documentation, .NET project) |
| `Makefile` | build rules |
| `docs/wiki/` | wiki pages, published to the Gitea wiki by `.gitea/workflows/publish-wiki.yml` |

## Documentation

See the wiki: Description, Results and Build and run.

## License

See [LICENSE](LICENSE).
