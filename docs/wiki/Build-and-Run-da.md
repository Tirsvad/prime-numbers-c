# Byg og kør

🌐 [English](Build-and-Run) · **Dansk** · [Bahasa Melayu](Build-and-Run-ms)

`Makefile` ligger i repositoriets rod og lægger programmet i `dist/`. Du skal
bruge `gcc` (Linux, eller MinGW-w64 / MSYS2 på Windows) og en CPU med AVX2.

```bash
make
```

```bash
make run
```

```bash
make clean
```

Sådan gemmer du primtallene og stadig ser tiden: omdiriger kun standard output.

```bash
./dist/prime_avx2 > primes.txt
```
