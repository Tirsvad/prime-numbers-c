# Bina dan jalankan

🌐 [English](Build-and-Run) · [Dansk](Build-and-Run-da) · **Bahasa Melayu**

`Makefile` berada di akar repositori dan meletakkan ketiga-tiga fail boleh laksana
(`prime_scalar`, `prime_avx2` dan `prime_sieve`) dalam `dist/`. Anda memerlukan `gcc` (Linux, atau MinGW-w64 / MSYS2 pada Windows) dan
CPU dengan AVX2.

```bash
make
```

```bash
make run
```

`make run` menjalankan `prime_scalar`, `prime_avx2` dan kemudian `prime_sieve`.

```bash
make clean
```

Untuk menyimpan nombor perdana dan masih melihat masa, alihkan standard output
sahaja:

```bash
./dist/prime_sieve > primes.txt
```

## Rust dan C#

Tiga algoritma yang sama juga ada dalam Rust dan C#, sebagai satu program setiap
satu yang memilih algoritma daripada argumen pertamanya. `make` tidak
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

Gunakan `scalar`, `avx2` atau `sieve` sebagai argumen. Rust memerlukan `cargo`
dan C# memerlukan .NET SDK (10). Output binaan berada dalam `src/rust/target/`
serta `src/csharp/bin/` dan `src/csharp/obj/`, yang diabaikan oleh git.
