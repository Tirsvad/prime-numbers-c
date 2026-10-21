# Byg og kør

🌐 [English](Build-and-Run) · **Dansk** · [Bahasa Melayu](Build-and-Run-ms)

`Makefile` ligger i repositoriets rod og lægger alle seks programmer
(`prime_scalar`, `prime_scalar_mt`, `prime_avx2`, `prime_avx2_mt`, `prime_sieve`
og `prime_sieve_thread`) i `dist/`. Du skal
bruge `gcc` (Linux, eller MinGW-w64 / MSYS2 på Windows) og en CPU med AVX2.

```bash
make
```

```bash
make run
```

`make run` kører alle seks efter hinanden. `_mt`-programmerne bruger OpenMP
(`-fopenmp`) og kører på alle CPU-tråde; `OMP_NUM_THREADS=4` sætter et andet
antal. Hvis de er langsommere end de serielle programmer på din maskine, så sæt
`OMP_WAIT_POLICY=passive` (se [Resultater](Results-da), "Parallelle versioner").

```bash
make clean
```

Sådan gemmer du primtallene og stadig ser tiden: omdiriger kun standard output.

```bash
./dist/prime_sieve > primes.txt
```

## Rust og C#

De samme algoritmer findes også i Rust og C#, som ét program hver, der vælger
algoritmen ud fra sit første argument. `make` bygger dem ikke.

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

Brug `scalar`, `scalar-mt`, `avx2`, `avx2-mt` eller `sieve` som argument. Rust kræver `cargo`, og C#
kræver .NET SDK (10). Build-output havner i `src/rust/target/` samt
`src/csharp/bin/` og `src/csharp/obj/`, som git ignorerer.

## GPU (CUDA)

`prime_cuda` kører på et NVIDIA-GPU. Det kræver NVIDIA-driveren og CUDA-toolkittet
(`nvcc`), så det er ikke en del af `make`. Byg og kør det med:

```bash
make cuda
```

```bash
./dist/prime_cuda
```

Hvis `nvcc` ikke er i din `PATH`, så angiv dens placering, fx
`make cuda NVCC=/usr/local/cuda-13.3/bin/nvcc`. Bygget bruger `-arch=native`, som
bygger til GPU'en i maskinen.
