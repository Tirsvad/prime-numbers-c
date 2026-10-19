# Penerangan

🌐 [English](Description) · [Dansk](Description-da) · **Bahasa Melayu**

## Tujuan

Program ini menunjukkan betapa lajunya sesebuah CPU. Kelajuan yang kelihatan
bergantung pada cara program ditulis, jadi angka itu mengukur CPU dan kod
bersama-sama, bukan CPU sahaja.

## Apa yang dilakukannya

Program menguji setiap nombor dari 0 hingga 1,000,000 dengan `is_prime_avx2`,
yang menggunakan pembahagian cuba-cuba AVX2 dengan 6k ± 1 (empat pembahagi
setiap langkah). Ia mencetak setiap nombor perdana pada barisnya sendiri dan
menulis baris ringkasan ke standard error:

```text
Found 78498 primes from 0 to 1000000 in 13.806 ms
```

78,498 ialah bilangan nombor perdana yang diketahui hingga 1,000,000, jadi baris
ringkasan juga berfungsi sebagai semakan ketepatan.

## Mengapa masa bergantung pada kod

CPU yang sama memberi masa yang sangat berbeza untuk program yang berbeza:

- algoritma (pembahagian cuba-cuba, ayak, ayak bersegmen),
- set arahan yang digunakan (kod skalar, AVX2, AVX-512),
- pengkompil dan bendera (`-O3 -mavx2`, MSVC `/O2 /arch:AVX2`),
- apa yang diukur (carian sahaja, atau juga pencetakan ke konsol).

Di sini hanya carian diukur. Keputusan dikumpul dahulu dan dicetak kemudian,
supaya kelajuan konsol tidak mengubah angka.
