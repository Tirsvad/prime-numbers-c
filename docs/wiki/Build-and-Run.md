# Build and run

**English**

The `Makefile` is in the repository root and puts the executable in `dist/`.
It needs `gcc` (Linux, or MinGW-w64 / MSYS2 on Windows) and a CPU with AVX2.

```bash
make
```

```bash
make run
```

```bash
make clean
```

To save the primes and still see the time, redirect only standard output:

```bash
./dist/prime_avx2 > primes.txt
```
