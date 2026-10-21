# prime-numbers-c

Lists every prime number from 0 to 1,000,000 and reports how long the search
took. It shows how fast a CPU is, but the result depends on how the program is
written: algorithm, instruction set (here AVX2), compiler and flags.

Three programs do the same job in three ways, so the code is easy to compare:

- `prime_scalar` tests each number with plain trial division (no AVX2).
- `prime_avx2` tests each number with AVX2 trial division, four divisors per step.
- `prime_sieve` uses a sieve of Eratosthenes over odd numbers, stored as a bit array.

Example results, the average of 10 runs on a 13th Gen Intel(R) Core(TM)
i5-13600K. They were built and run under WSL 2 (Debian, gcc 12.2, `-O3`;
`-mavx2` for all but `prime_scalar`), see the wiki page Results:

```text
prime_scalar: Found 78498 primes from 0 to 1000000 in 25.330 ms (average of 10 runs)
prime_avx2: Found 78498 primes from 0 to 1000000 in 12.856 ms (average of 10 runs)
prime_sieve: Found 78498 primes from 0 to 1000000 in 1.103 ms (average of 10 runs)
```

## Build and run

Needs `gcc` (Linux, or MinGW-w64 / MSYS2 on Windows), `make` and a CPU with AVX2.

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
(use `prime_scalar`, `prime_avx2` or `prime_sieve`):

```bash
./dist/prime_sieve > primes.txt
```

The Rust and C# versions are kept for comparison and are not built by `make`; see
the wiki page Build and run.

## Layout

| Path | Content |
| --- | --- |
| `src/prime_scalar.c` | plain scalar trial-division program |
| `src/prime_avx2.c` | AVX2 trial-division program |
| `src/prime_sieve.c` | sieve of Eratosthenes program |
| `src/rust/` | the same three algorithms in Rust (documentation, Cargo project) |
| `src/csharp/` | the same three algorithms in C# (documentation, .NET project) |
| `Makefile` | build rules |
| `docs/wiki/` | wiki pages, published to the Gitea wiki by `.gitea/workflows/publish-wiki.yml` |

## Documentation

See the wiki: Description, Results and Build and run.

## License

See [LICENSE](LICENSE).
