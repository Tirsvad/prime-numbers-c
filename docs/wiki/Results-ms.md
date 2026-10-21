# Keputusan

🌐 [English](Results) · [Dansk](Results-da) · **Bahasa Melayu**

Masa untuk mencari semua nombor perdana dari 0 hingga 1,000,000. Hanya carian
diukur.

Setiap masa ialah purata 10 larian.

| CPU | Program | Keputusan |
| --- | --- | --- |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar` | Found 78498 primes from 0 to 1000000 in 46.692 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2` | Found 78498 primes from 0 to 1000000 in 15.841 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_sieve` | Found 78498 primes from 0 to 1000000 in 1.191 ms |

AVX2 kira-kira 3 kali lebih pantas daripada versi skalar, dan ayak kira-kira 13
kali lebih pantas daripada AVX2 (kira-kira 39 kali lebih pantas daripada
skalar).

## Cara keputusan diukur

Program dibina dengan `make` dan dijalankan di bawah WSL 2 (Windows Subsystem
for Linux) pada Windows 11: Debian dengan gcc 12.2 dan kernel Linux 6.18. WSL
menjalankan kernel Linux sebenar dalam mesin maya ringan pada 13th Gen
Intel(R) Core(TM) i5-13600K yang sama, jadi CPU sama dengan hos. Bendera
pengkompil: `-O3 -mavx2` untuk `prime_avx2` dan `prime_sieve`, dan `-O3` tanpa
`-mavx2` untuk `prime_scalar`, supaya pengkompil tidak boleh menggunakan AVX2
di situ. Setiap program dimulakan 10 kali dan jadual menunjukkan purata.
Masa berbeza beberapa milisaat antara larian, dan setiap larian
disertakan dalam purata.

Masa di bawah WSL boleh berbeza sedikit daripada Linux asli atau Windows, jadi
bandingkan angka hanya apabila diukur dengan cara yang sama.

Keputusan hanya boleh dibandingkan dengan yang lain apabila kod, pengkompil dan
bendera sama. Untuk menambah keputusan, jalankan program pada mesin anda (lihat
[Bina dan jalankan](Build-and-Run-ms)) dan tambah satu baris dengan nama CPU
anda dan baris ringkasan.
