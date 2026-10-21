# Keputusan

🌐 [English](Results) · [Dansk](Results-da) · **Bahasa Melayu**

Masa untuk mencari semua nombor perdana dari 0 hingga 1,000,000. Hanya carian
diukur.

Setiap masa ialah purata 10 larian.

| CPU | Program | Keputusan |
| --- | --- | --- |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar` | Found 78498 primes from 0 to 1000000 in 25.330 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2` | Found 78498 primes from 0 to 1000000 in 12.856 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_sieve` | Found 78498 primes from 0 to 1000000 in 1.103 ms |

AVX2 kira-kira 2 kali lebih pantas daripada versi skalar, dan ayak kira-kira 12
kali lebih pantas daripada AVX2 (kira-kira 23 kali lebih pantas daripada
skalar).

## Bahasa lain

Tiga algoritma yang sama juga ditulis dalam Rust (`src/rust/`) dan C#
(`src/csharp/`). Ia disimpan sebagai dokumentasi, supaya kod dalam ketiga-tiga
bahasa boleh dibaca sebelah-menyebelah. Setiap satu mencetak 78,498 nombor
perdana yang sama seperti program C. Masa larian, purata 10 larian di bawah WSL
pada CPU yang sama:

| Bahasa | `scalar` | `avx2` | `sieve` |
| --- | --- | --- | --- |
| C (gcc 12.2) | 25.330 ms | 12.856 ms | 1.103 ms |
| Rust 1.99.1 | 30.580 ms | 16.358 ms | 1.439 ms |
| C# (.NET 10) | 51.949 ms | 18.243 ms | 1.911 ms |

Susunannya sama dalam ketiga-tiga bahasa: ayak paling pantas dengan jauh,
kemudian AVX2, kemudian skalar. C# lebih perlahan dalam program skalar dan
sedikit lebih perlahan dalam ayak. Perbezaan antara C dan Rust adalah kecil, dan
apabila keseluruhan pengukuran diulang pada masa lain, purata berubah sehingga
kira-kira 20% (contohnya C skalar antara 25 dan 29 ms), jadi perbezaan sebesar
itu tidak patut dibaca sebagai perbezaan sebenar. Program berbeza dalam lebih
daripada bahasa (pengkompil, runtime dan butiran pustaka), jadi baca jadual
sebagai perbandingan kasar.

- **Rust** 1.99.1, `cargo build --release` (`opt-level = 3`). Versi AVX2
  menggunakan intrinsik `std::arch` dengan `#[target_feature(enable = "avx2")]`.
- **C#** pada .NET 10, binaan Release dengan kompilasi berperingkat dimatikan,
  supaya JIT terus menghasilkan kod yang dioptimumkan sepenuhnya. Versi AVX2
  menggunakan `System.Runtime.Intrinsics`. Masa JIT bagi kod yang diukur
  termasuk dalam masa yang diukur, dan permulaan runtime .NET tidak.

### Mengapa lebar pembahagian penting untuk program skalar

Program skalar menghabiskan masanya pada pembahagian `n % d`. Dalam versi
pertama program C, `d` ialah `uint64_t`, dan gcc menukar `n % d` kepada
pembahagian 64-bit (`divq`). Versi itu mengambil kira-kira 46.7 ms, manakala
program Rust mengambil kira-kira 27 ms dalam perbandingan yang sama. Sebabnya
ialah pengkompil Rust (LLVM) melihat bahawa `n` dan `d` muat dalam 32 bit dan
menggunakan pembahagian 32-bit yang lebih murah (`divl`); pada CPU ini
pembahagian 64-bit lebih perlahan daripada 32-bit.

`prime_scalar.c` kini mengisytiharkan `d` sebagai `uint32_t` (hanya
`(uint64_t)d * d` dikira dalam 64 bit, jadi had tidak boleh melimpah). C
kemudian sama pantas dengan Rust. Jadual menunjukkan versi ini.

## Masa bina dan saiz fail

Masa bina ialah purata 5 binaan bersih (output binaan dipadam dengan `rm -rf`
sebelum setiap satu) di bawah WSL. Saiz fail ialah saiz fail yang dibina.

| | Masa bina | Saiz fail | Distrip |
| --- | --- | --- | --- |
| C `prime_scalar` (gcc `-O3`) | 207 ms | 16,136 bait | 14,496 bait |
| C `prime_avx2` (gcc `-O3 -mavx2`) | 416 ms | 16,216 bait | 14,504 bait |
| C `prime_sieve` (gcc `-O3 -mavx2`) | 237 ms | 16,224 bait | 14,504 bait |
| C ketiga-tiganya dengan `make` | 1,144 ms | 48,576 bait | |
| Rust `cargo build --release` (ketiga-tiganya) | 397 ms | 491,824 bait | 376,288 bait |
| C# `dotnet build -c Release` (ketiga-tiganya) | 990 ms | `prime.dll` 7,680 bait | |

- **Satu program atau tiga.** C membina tiga program. Program Rust dan C#
  mengandungi ketiga-tiga algoritma dan memilih satu daripada argumen pertama
  (`prime scalar|avx2|sieve`), jadi satu binaan meliputi ketiga-tiganya. Baris
  "ketiga-tiganya" ialah yang patut dibandingkan.
- **Rust lebih besar** kerana pustaka standard dipautkan ke dalam program
  (kira-kira 376 KB selepas `strip`). Program C menggunakan pustaka C sistem,
  yang tidak dikira dalam saiznya.
- **C#** membina `prime.dll` yang kecil (7,680 bait; 102,847 bait untuk seluruh
  folder output dengan `prime.pdb` dan fail `.json`). Ia tidak mengandungi
  runtime .NET, yang mesti dipasang untuk menjalankannya. Binaan C# pertama
  paling perlahan (1,732 ms berbanding kira-kira 800 ms untuk empat yang lain),
  kerana pelayan binaan bermula ketika itu.
- **Masa bina** adalah untuk program tanpa kebergantungan lain. Program yang
  lebih besar, binaan tokokan atau cache sejuk memberi angka lain.

## Cara keputusan diukur

Program C dibina dengan `make` dan program Rust dan C# dengan arahan dalam [Bina dan jalankan](Build-and-Run-ms), dan
semuanya dijalankan di bawah WSL 2 (Windows Subsystem for Linux) pada Windows
11: Debian dengan gcc 12.2 dan kernel Linux 6.18. WSL menjalankan kernel Linux
sebenar dalam mesin maya ringan pada 13th Gen Intel(R) Core(TM) i5-13600K yang
sama, jadi CPU sama dengan hos. Bendera pengkompil: `-O3 -mavx2` untuk
`prime_avx2` dan `prime_sieve`, dan `-O3` tanpa `-mavx2` untuk `prime_scalar`,
supaya pengkompil tidak boleh menggunakan AVX2 di situ. Setiap program dimulakan
10 kali dan jadual menunjukkan purata. Masa berbeza beberapa milisaat antara
larian, dan setiap larian disertakan dalam purata.

Masa di bawah WSL boleh berbeza sedikit daripada Linux asli atau Windows, jadi
bandingkan angka hanya apabila diukur dengan cara yang sama.

Keputusan hanya boleh dibandingkan dengan yang lain apabila kod, pengkompil dan
bendera sama. Untuk menambah keputusan, jalankan program pada mesin anda (lihat
[Bina dan jalankan](Build-and-Run-ms)) dan tambah satu baris bagi setiap program
dengan nama CPU anda dan baris ringkasan.
