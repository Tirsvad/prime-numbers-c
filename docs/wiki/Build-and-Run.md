# Build and run

🌐 **English** · [Dansk](Build-and-Run-da) · [Bahasa Melayu](Build-and-Run-ms)

The `Makefile` is in the repository root and puts all three executables
(`prime_scalar`, `prime_avx2` and `prime_sieve`) in `dist/`.
It needs `gcc` (Linux, or MinGW-w64 / MSYS2 on Windows) and a CPU with AVX2.

```bash
make
```

```bash
make run
```

`make run` runs `prime_scalar`, `prime_avx2` and then `prime_sieve`.

```bash
make clean
```

To save the primes and still see the time, redirect only standard output:

```bash
./dist/prime_sieve > primes.txt
```
