# Penerangan

🌐 [English](Description) · [Dansk](Description-da) · **Bahasa Melayu**

## Tujuan

Program ini menunjukkan betapa lajunya sesebuah CPU. Kelajuan yang kelihatan
bergantung pada cara program ditulis, jadi angka itu mengukur CPU dan kod
bersama-sama, bukan CPU sahaja.

## Apa yang dilakukannya

Program mencari semua nombor perdana dari 0 hingga 1,000,000 dengan beberapa
cara, dalam program berasingan supaya kod mudah dibandingkan:

- `src/prime_scalar.c` membina `prime_scalar`. Ia menguji setiap nombor dengan
  pembahagian cuba-cuba biasa dengan 6k ± 1, satu pembahagi setiap langkah,
  tanpa AVX2.
- `src/prime_avx2.c` membina `prime_avx2`. Ia menguji setiap nombor dengan
  `is_prime_avx2`, pembahagian cuba-cuba yang sama dengan AVX2 (empat pembahagi
  setiap langkah). Lihat [Cara AVX2 berfungsi](How-AVX2-Works-ms).
- `src/prime_scalar_mt.c` dan `src/prime_avx2_mt.c` membina `prime_scalar_mt`
  dan `prime_avx2_mt`: dua program yang sama, dengan nombor dibahagikan kepada
  semua thread CPU dengan OpenMP. Lihat [Keputusan](Results-ms) ("Versi selari").
- `src/prime_sieve.c` membina `prime_sieve`. Ia menggunakan ayak Eratosthenes
  ke atas nombor ganjil, disimpan sebagai tatasusunan bit (kira-kira 62 KB,
  jadi ia kekal dalam cache CPU). Lihat [Cara ayak berfungsi](How-Sieve-Works-ms).
- `src/prime_sieve_thread.c` membina `prime_sieve_thread`: ayak yang sama pada
  thread POSIX latar belakang. `src/prime_cuda.cu` membina `prime_cuda`: pembahagian
  cuba-cuba pada GPU NVIDIA. Lihat [Keputusan](Results-ms) ("Thread latar belakang dan GPU").

Semuanya mencetak setiap nombor perdana pada barisnya sendiri dan menulis
baris ringkasan ke standard error (program `_mt` memulakannya dengan bilangan
thread):

```text
prime_scalar: Found 78498 primes from 0 to 1000000 in 27.675 ms (purata 10 larian)
prime_scalar_mt: [20 threads] Found 78498 primes from 0 to 1000000 in 4.790 ms (purata 10 larian)
prime_avx2: Found 78498 primes from 0 to 1000000 in 13.804 ms (purata 10 larian)
prime_avx2_mt: [20 threads] Found 78498 primes from 0 to 1000000 in 3.814 ms (purata 10 larian)
prime_sieve: Found 78498 primes from 0 to 1000000 in 1.183 ms (purata 10 larian)
```

Algoritma yang sama juga ditulis dalam Rust (`src/rust/`) dan C# (`src/csharp/`) untuk
perbandingan; lihat [Keputusan](Results-ms) untuk masa, masa bina dan saiz fail mereka.

Masa ialah purata 10 larian di bawah WSL (lihat [Keputusan](Results-ms)). AVX2
menjadikan pembahagian cuba-cuba kira-kira 2 kali lebih pantas daripada versi
skalar, dan thread menjadikan setiap satu 3.6 hingga 5.8 kali lebih pantas lagi.
Ayak bersiri masih lebih pantas daripada kesemuanya, kerana ia tidak menguji
setiap nombor secara berasingan. Kelima-limanya mencetak nombor perdana yang
sama.

## Mengapa masa bergantung pada kod

CPU yang sama memberi masa yang sangat berbeza untuk program yang berbeza:

- algoritma (pembahagian cuba-cuba, ayak, ayak bersegmen),
- set arahan yang digunakan (kod skalar, AVX2, AVX-512),
- pengkompil dan bendera (`-O3 -mavx2`, MSVC `/O2 /arch:AVX2`),
- apa yang diukur (carian sahaja, atau juga pencetakan ke konsol).

Di sini hanya carian diukur. Keputusan dikumpul dahulu dan dicetak kemudian,
supaya kelajuan konsol tidak mengubah angka.
