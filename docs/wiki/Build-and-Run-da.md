# Byg og kør

🌐 [English](Build-and-Run) · **Dansk** · [Bahasa Melayu](Build-and-Run-ms)

`Makefile` ligger i repositoriets rod og lægger alle tre programmer
(`prime_scalar`, `prime_avx2` og `prime_sieve`) i `dist/`. Du skal
bruge `gcc` (Linux, eller MinGW-w64 / MSYS2 på Windows) og en CPU med AVX2.

```bash
make
```

```bash
make run
```

`make run` kører `prime_scalar`, `prime_avx2` og derefter `prime_sieve`.

```bash
make clean
```

Sådan gemmer du primtallene og stadig ser tiden: omdiriger kun standard output.

```bash
./dist/prime_sieve > primes.txt
```
