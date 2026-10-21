# Keputusan

🌐 [English](Results) · [Dansk](Results-da) · **Bahasa Melayu**

Masa untuk mencari semua nombor perdana dari 0 hingga 1,000,000. Hanya carian
diukur.

Setiap masa ialah purata 10 larian.

| CPU | Program | Thread | Keputusan |
| --- | --- | --- | --- |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar` | 1 thread | Found 78498 primes from 0 to 1000000 in 27.675 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar_mt` | 20 thread | Found 78498 primes from 0 to 1000000 in 4.790 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2` | 1 thread | Found 78498 primes from 0 to 1000000 in 13.804 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2_mt` | 20 thread | Found 78498 primes from 0 to 1000000 in 3.814 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_sieve` | 1 thread | Found 78498 primes from 0 to 1000000 in 1.183 ms |

AVX2 kira-kira 2 kali lebih pantas daripada versi skalar. Program `_mt`
berjalan pada semua 20 thread CPU (lihat di bawah). Ayak paling pantas
daripada semuanya, malah lebih pantas daripada pembahagian cuba-cuba pada 20
thread, kerana ia melakukan kerja yang jauh lebih sedikit.

## Versi selari

`prime_scalar_mt` dan `prime_avx2_mt` melakukan pembahagian cuba-cuba yang sama
seperti `prime_scalar` dan `prime_avx2`, tetapi pada semua thread CPU dengan
OpenMP. Setiap nombor diuji secara berasingan, jadi gelung boleh dibahagikan:
setiap thread mengambil blok seterusnya sebanyak 1,024 nombor apabila ia bebas
(`schedule(dynamic, 1024)`), kerana nombor yang lebih besar lebih mahal untuk
diuji. Setiap nombor menulis bait sendiri dalam tatasusunan bendera, jadi thread
tidak pernah menyentuh memori yang sama, dan nombor perdana dikumpulkan
kemudian mengikut tertib pada satu thread, jadi output tersusun. Masa termasuk
langkah terakhir itu. Ayak tidak dijadikan selari: ia hanya mengambil kira-kira
1 ms, lebih kurang kos memulakan thread.

`prime_avx2_mt` kira-kira 3.6 kali lebih pantas daripada `prime_avx2`, dan
`prime_scalar_mt` kira-kira 5.8 kali lebih pantas daripada `prime_scalar`, pada
20 thread. Itu jauh daripada 20 kali. Beginilah masa menurun mengikut bilangan
thread (`OMP_NUM_THREADS`, tunggu pasif, purata 10 larian):

| Thread | `prime_scalar_mt` | `prime_avx2_mt` |
| --- | --- | --- |
| 1 | 28.916 ms | 15.069 ms |
| 2 | 16.637 ms | 8.969 ms |
| 4 | 8.251 ms | 4.808 ms |
| 8 | 5.746 ms | 4.014 ms |
| 14 | 4.960 ms | 3.602 ms |
| 20 | 4.910 ms | 3.490 ms |

- Masa berhenti menurun selepas kira-kira 4 hingga 8 thread. Sebahagian kerja
  tidak selari (memulakan thread, mengumpul nombor perdana mengikut tertib), dan
  CPU ini mempunyai 6 teras pantas dengan 2 thread setiap satu serta 8 teras yang
  lebih perlahan, jadi thread terakhir memberi sedikit.
- **Dasar menunggu.** Secara lalai thread OpenMP yang kehabisan kerja berpusing
  dan menunggu. Dengan semua 20 thread sibuk di bawah WSL ini menjadikan program
  perlahan dan tidak stabil (antara 10 dan 20 ms dalam tiga pengukuran,
  berbanding 3.5 hingga 4.9 ms). Jadual di atas dan jadual utama menggunakan
  `OMP_WAIT_POLICY=passive`, di mana thread yang menunggu tidur. Jika program
  `_mt` lebih perlahan daripada yang bersiri pada mesin anda, cuba tetapkannya:

```bash
OMP_WAIT_POLICY=passive ./dist/prime_avx2_mt
```

## Thread latar belakang dan GPU

Dua lagi program, diukur dalam sesi yang sama dengan jadual di atas, di sebelah
program C bersiri dan `_mt` untuk perbandingan:

| Program | Purata 10 larian |
| --- | --- |
| `prime_scalar` | 27.675 ms |
| `prime_avx2` | 13.804 ms |
| `prime_sieve` | 1.183 ms |
| `prime_sieve_thread` (thread latar belakang) | 1.268 ms |
| `prime_scalar_mt` (20 thread, tunggu pasif) | 4.790 ms |
| `prime_avx2_mt` (20 thread, tunggu pasif) | 3.814 ms |
| `prime_cuda` (RTX 3050) | 5.495 ms |

### Ayak thread latar belakang (POSIX)

`prime_sieve_thread` (`src/prime_sieve_thread.c`) menjalankan ayak yang sama
seperti `prime_sieve`, tetapi pada thread latar belakang: thread utama
memulakannya dengan `pthread_create`, bebas melakukan kerja lain, dan di sini
hanya menunggu hasilnya dengan `pthread_join`. Satu thread tambahan melakukan
kerja yang sama seperti ayak bersiri, jadi ia tidak boleh lebih pantas;
memulakan dan menggabungkan thread itu berharga kira-kira 0.09 ms (1.268 ms
berbanding 1.183 ms). Tujuan bentuk ini ialah memastikan program kekal responsif
semasa kerja berjalan, bukan mempercepatkan kerja. Ia memerlukan thread POSIX,
jadi ia dibina dengan `-pthread`.

### GPU (CUDA)

`prime_cuda` (`src/prime_cuda.cu`) menjalankan pembahagian cuba-cuba pada GPU
NVIDIA: satu thread GPU menguji satu nombor, menulis satu bait bendera, dan CPU
mengumpul nombor perdana mengikut tertib. Ia dibina dengan `nvcc` (`make cuda`)
dan bukan sebahagian daripada `make`. GPU di sini ialah NVIDIA GeForce RTX 3050,
dicapai dari WSL melalui pemacu Windows, dengan CUDA 13.3.

- Masa dalam jadual, 5.5 ms, termasuk memperuntukkan memori GPU, menjalankan
  kernel, menyalin bendera kembali dan mengumpul nombor perdana. Kernel itu
  sendiri mengambil kira-kira 2.5 ms.
- Ia **tidak** termasuk permulaan sekali sahaja runtime CUDA, yang mengambil 130
  hingga 170 ms dalam larian ini (panggilan CUDA pertama). Program
  mencetaknya secara berasingan.
- GPU lebih perlahan daripada program CPU 20 thread (3.8 hingga 4.8 ms) dan
  daripada ayak (1.2 ms) pada saiz ini. Terdapat sedikit kerja (1,000,000
  nombor) untuk beribu-ribu thread GPU, dan kos salinan dan permulaan adalah
  tetap. Kernel mungkin juga dihalang kerana 32 nombor jiran berjalan serentak
  dan satu kumpulan sama perlahan dengan nombor paling perlahannya: nombor
  perdana memerlukan kira-kira 160 langkah gelung manakala kebanyakan nombor
  komposit berhenti serta-merta. Ini tidak diukur. Had yang jauh lebih besar akan
  memihak kepada GPU, tetapi itu tidak dicuba.

## Bahasa lain

Algoritma yang sama juga ditulis dalam Rust (`src/rust/`) dan C#
(`src/csharp/`). Ia disimpan sebagai dokumentasi, supaya kod dalam ketiga-tiga
bahasa boleh dibaca sebelah-menyebelah. Setiap satu mencetak 78,498 nombor
perdana yang sama seperti program C. Masa larian, purata 10 larian di bawah WSL
pada CPU yang sama; `-mt` ialah versi selari pada semua 20 thread:

| Bahasa | `scalar` | `scalar-mt` | `avx2` | `avx2-mt` | `sieve` |
| --- | --- | --- | --- | --- | --- |
| C (gcc 12.2) | 27.675 ms | 4.790 ms | 13.804 ms | 3.814 ms | 1.183 ms |
| Rust 1.99.1 | 29.020 ms | 5.413 ms | 14.580 ms | 4.415 ms | 1.294 ms |
| C# (.NET 10) | 48.883 ms | 18.855 ms | 16.704 ms | 17.894 ms | 1.850 ms |

Susunannya sama dalam ketiga-tiga bahasa: ayak paling pantas dengan jauh, dan
AVX2 lebih pantas daripada skalar. C dan Rust hampir sama; apabila keseluruhan
pengukuran diulang pada masa lain, purata berubah sehingga kira-kira 20%
(contohnya C skalar antara 25 dan 29 ms), jadi perbezaan sebesar itu tidak patut
dibaca sebagai perbezaan sebenar. Program berbeza dalam lebih daripada bahasa
(pengkompil, runtime dan butiran pustaka), jadi baca jadual sebagai perbandingan
kasar.

- **Rust** 1.99.1, `cargo build --release` (`opt-level = 3`). Versi AVX2
  menggunakan intrinsik `std::arch` dengan `#[target_feature(enable = "avx2")]`.
  Versi `-mt` menggunakan `std::thread::scope` dengan pembilang blok
  dikongsi dan tiada crate tambahan.
- **C#** pada .NET 10, binaan Release dengan kompilasi berperingkat dimatikan,
  supaya JIT terus menghasilkan kod yang dioptimumkan sepenuhnya. Versi AVX2
  menggunakan `System.Runtime.Intrinsics`. Versi `-mt` menggunakan
  `Parallel.For`. Masa JIT bagi kod yang diukur termasuk dalam masa yang diukur,
  dan permulaan runtime .NET tidak.
- **C# perlahan dalam mod `-mt`** (kira-kira 18 ms, tidak lebih baik daripada
  AVX2 bersiri). `Parallel.For` pertama dalam sesuatu proses membayar kos
  permulaan kira-kira 14 ms (kolam thread dan permulaan pustaka). Memanggilnya
  semula dalam proses yang sama mengambil 3 hingga 4 ms, sama pantas dengan Rust,
  begitu juga gelung thread biasa yang dimulakan secara manual. Ini disemak dalam
  eksperimen sekali pakai. Program yang hanya mempunyai satu kerja selari yang
  pendek seperti di sini membayar kos itu setiap kali.

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
| C `prime_scalar_mt` (`-O3 -fopenmp`) | 189 ms | 16,720 bait | 14,528 bait |
| C `prime_avx2` (`-O3 -mavx2`) | 416 ms | 16,216 bait | 14,504 bait |
| C `prime_avx2_mt` (`-O3 -mavx2 -fopenmp`) | 396 ms | 16,808 bait | 14,536 bait |
| C `prime_sieve` (`-O3 -mavx2`) | 237 ms | 16,224 bait | 14,504 bait |
| C kelima-limanya (jumlah lima baris) | 1,445 ms | 82,104 bait | |
| C `prime_sieve_thread` (`-O3 -pthread`) | 206 ms | 16,432 bait | 14,528 bait |
| C `prime_cuda` (`nvcc -O3 -arch=native`) | 820 ms | 1,087,000 bait | 769,904 bait |
| Rust `cargo build --release` (kelima-limanya) | 521 ms | 563,504 bait | 428,184 bait |
| C# `dotnet build -c Release` (kelima-limanya) | 1,069 ms | `prime.dll` 8,704 bait | |

- **`prime_cuda` lebih besar** (kira-kira 1 MB) kerana runtime CUDA dipautkan
  secara statik, dan ia mengandungi kod GPU untuk GPU mesin ini. Ia memerlukan
  pemacu NVIDIA untuk berjalan.
- **Satu program atau lima.** C membina lima program. Program Rust dan C#
  mengandungi kelima-lima mod dan memilih satu daripada argumen pertama
  (`prime scalar|scalar-mt|avx2|avx2-mt|sieve`), jadi satu binaan meliputi
  semuanya. Baris "kelima-limanya" ialah yang patut dibandingkan.
- **Rust lebih besar** kerana pustaka standard dipautkan ke dalam program
  (kira-kira 428 KB selepas `strip`). Program C menggunakan pustaka C sistem,
  yang tidak dikira dalam saiznya. Program `-fopenmp` memautkan pustaka OpenMP
  (`libgomp`) secara dinamik, jadi ia juga tidak dikira.
- **C#** membina `prime.dll` yang kecil (8,704 bait; 104,339 bait untuk seluruh
  folder output dengan `prime.pdb` dan fail `.json`). Ia tidak mengandungi
  runtime .NET, yang mesti dipasang untuk menjalankannya. Binaan C# pertama
  paling perlahan (1,974 ms berbanding kira-kira 850 ms untuk empat yang lain),
  kerana pelayan binaan bermula ketika itu.
- **Masa bina** adalah untuk program tanpa kebergantungan lain. Program yang
  lebih besar, binaan tokokan atau cache sejuk memberi angka lain.

## Cara keputusan diukur

Program C dibina dengan `make` dan program Rust dan C# dengan arahan dalam
[Bina dan jalankan](Build-and-Run-ms), dan semuanya dijalankan di bawah WSL 2
(Windows Subsystem for Linux) pada Windows 11: Debian dengan gcc 12.2 dan kernel
Linux 6.18. WSL menjalankan kernel Linux sebenar dalam mesin maya ringan pada
13th Gen Intel(R) Core(TM) i5-13600K yang sama (20 thread), jadi CPU sama dengan
hos. Bendera pengkompil: `-O3 -mavx2` untuk `prime_avx2` dan `prime_sieve`, `-O3`
tanpa `-mavx2` untuk `prime_scalar` supaya pengkompil tidak boleh menggunakan
AVX2 di situ, dan `-fopenmp` untuk program `_mt`. Setiap program dimulakan 10
kali dan jadual menunjukkan purata. Masa berbeza beberapa milisaat antara
larian, dan setiap larian disertakan dalam purata.

Masa di bawah WSL boleh berbeza sedikit daripada Linux asli atau Windows, jadi
bandingkan angka hanya apabila diukur dengan cara yang sama.

Keputusan hanya boleh dibandingkan dengan yang lain apabila kod, pengkompil dan
bendera sama. Untuk menambah keputusan, jalankan program pada mesin anda (lihat
[Bina dan jalankan](Build-and-Run-ms)) dan tambah satu baris bagi setiap program
dengan nama CPU anda, bilangan thread dan baris ringkasan.
