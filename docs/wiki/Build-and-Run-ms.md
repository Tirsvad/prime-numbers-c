# Bina dan jalankan

🌐 [English](Build-and-Run) · [Dansk](Build-and-Run-da) · **Bahasa Melayu**

`Makefile` berada di akar repositori dan meletakkan keenam-enam fail boleh laksana
(`prime_scalar`, `prime_scalar_mt`, `prime_avx2`, `prime_avx2_mt`, `prime_sieve`
dan `prime_sieve_thread`) dalam `dist/`. Anda memerlukan `gcc` (Linux, atau MinGW-w64 / MSYS2 pada Windows) dan
CPU dengan AVX2.

```bash
make
```

```bash
make run
```

`make run` menjalankan keenam-enamnya satu demi satu. Program `_mt` menggunakan
OpenMP (`-fopenmp`) dan berjalan pada semua thread CPU; `OMP_NUM_THREADS=4`
menetapkan bilangan lain. Jika ia lebih perlahan daripada program bersiri pada
mesin anda, tetapkan `OMP_WAIT_POLICY=passive` (lihat [Keputusan](Results-ms),
"Versi selari").

```bash
make clean
```

Untuk menyimpan nombor perdana dan masih melihat masa, alihkan standard output
sahaja:

```bash
./dist/prime_sieve > primes.txt
```

## Rust dan C#

Algoritma yang sama juga ada dalam Rust dan C#, sebagai satu program setiap satu
yang memilih algoritma daripada argumen pertamanya. `make` tidak
membinanya.

```bash
cargo build --release --manifest-path src/rust/Cargo.toml
```

```bash
./src/rust/target/release/prime sieve > primes.txt
```

```bash
dotnet build -c Release src/csharp/prime.csproj -o src/csharp/bin/out
```

```bash
dotnet src/csharp/bin/out/prime.dll sieve > primes.txt
```

Gunakan `scalar`, `scalar-mt`, `avx2`, `avx2-mt` atau `sieve` sebagai argumen. Rust memerlukan `cargo`
dan C# memerlukan .NET SDK (10). Output binaan berada dalam `src/rust/target/`
serta `src/csharp/bin/` dan `src/csharp/obj/`, yang diabaikan oleh git.

## GPU (CUDA)

`prime_cuda` berjalan pada GPU NVIDIA. Ia memerlukan pemacu NVIDIA dan toolkit
CUDA (`nvcc`), jadi ia bukan sebahagian daripada `make`. Bina dan jalankannya
dengan:

```bash
make cuda
```

```bash
./dist/prime_cuda
```

Jika `nvcc` tiada dalam `PATH` anda, nyatakan lokasinya, contohnya
`make cuda NVCC=/usr/local/cuda-13.3/bin/nvcc`. Binaan menggunakan
`-arch=native`, yang membina untuk GPU dalam mesin.
