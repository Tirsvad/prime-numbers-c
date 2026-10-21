# Penerangan

🌐 [English](Description) · [Dansk](Description-da) · **Bahasa Melayu**

## Tujuan

Program ini menunjukkan betapa lajunya sesebuah CPU. Kelajuan yang kelihatan
bergantung pada cara program ditulis, jadi angka itu mengukur CPU dan kod
bersama-sama, bukan CPU sahaja.

## Apa yang dilakukannya

Program mencari semua nombor perdana dari 0 hingga 1,000,000 dengan tiga cara,
dalam tiga program berasingan supaya kod mudah dibandingkan:

- `src/prime_scalar.c` membina `prime_scalar`. Ia menguji setiap nombor dengan
  pembahagian cuba-cuba biasa dengan 6k ± 1, satu pembahagi setiap langkah,
  tanpa AVX2.
- `src/prime_avx2.c` membina `prime_avx2`. Ia menguji setiap nombor dengan
  `is_prime_avx2`, pembahagian cuba-cuba yang sama dengan AVX2 (empat pembahagi
  setiap langkah). Lihat [Cara AVX2 berfungsi](How-AVX2-Works-ms).
- `src/prime_sieve.c` membina `prime_sieve`. Ia menggunakan ayak Eratosthenes
  ke atas nombor ganjil, disimpan sebagai tatasusunan bit (kira-kira 62 KB,
  jadi ia kekal dalam cache CPU).

Semuanya mencetak setiap nombor perdana pada barisnya sendiri dan menulis
baris ringkasan ke standard error:

```text
prime_scalar: Found 78498 primes from 0 to 1000000 in 46.692 ms (purata 10 larian)
prime_avx2: Found 78498 primes from 0 to 1000000 in 15.841 ms (purata 10 larian)
prime_sieve: Found 78498 primes from 0 to 1000000 in 1.191 ms (purata 10 larian)
```

Masa ialah purata 10 larian di bawah WSL (lihat [Keputusan](Results-ms)). AVX2
menjadikan pembahagian cuba-cuba kira-kira 3 kali lebih pantas daripada versi
skalar, dan ayak kira-kira 13 kali lebih pantas lagi kerana ia tidak menguji
setiap nombor secara berasingan. Ketiga-tiganya mencetak nombor perdana yang
sama.

## Mengapa masa bergantung pada kod

CPU yang sama memberi masa yang sangat berbeza untuk program yang berbeza:

- algoritma (pembahagian cuba-cuba, ayak, ayak bersegmen),
- set arahan yang digunakan (kod skalar, AVX2, AVX-512),
- pengkompil dan bendera (`-O3 -mavx2`, MSVC `/O2 /arch:AVX2`),
- apa yang diukur (carian sahaja, atau juga pencetakan ke konsol).

Di sini hanya carian diukur. Keputusan dikumpul dahulu dan dicetak kemudian,
supaya kelajuan konsol tidak mengubah angka.
