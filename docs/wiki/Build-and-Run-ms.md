# Bina dan jalankan

🌐 [English](Build-and-Run) · [Dansk](Build-and-Run-da) · **Bahasa Melayu**

`Makefile` berada di akar repositori dan meletakkan fail boleh laksana dalam
`dist/`. Anda memerlukan `gcc` (Linux, atau MinGW-w64 / MSYS2 pada Windows) dan
CPU dengan AVX2.

```bash
make
```

```bash
make run
```

```bash
make clean
```

Untuk menyimpan nombor perdana dan masih melihat masa, alihkan standard output
sahaja:

```bash
./dist/prime_avx2 > primes.txt
```
