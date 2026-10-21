# Byg og kør

🌐 [English](Build-and-Run) · **Dansk** · [Bahasa Melayu](Build-and-Run-ms)

`Makefile` ligger i repositoriets rod og lægger alle tre programmer
(`prime_scalar`, `prime_avx2` og `prime_sieve`) i `dist/`. Du skal
bruge `gcc` (Linux, eller MinGW-w64 / MSYS2 på Windows) og en CPU med AVX2.

```bash
make
```

```bash
make run
```

`make run` kører `prime_scalar`, `prime_avx2` og derefter `prime_sieve`.

```bash
make clean
```

Sådan gemmer du primtallene og stadig ser tiden: omdiriger kun standard output.

```bash
./dist/prime_sieve > primes.txt
```

## Rust og C#

De samme tre algoritmer findes også i Rust og C#, som ét program hver, der
vælger algoritmen ud fra sit første argument. `make` bygger dem ikke.

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

Brug `scalar`, `avx2` eller `sieve` som argument. Rust kræver `cargo`, og C#
kræver .NET SDK (10). Build-output havner i `src/rust/target/` samt
`src/csharp/bin/` og `src/csharp/obj/`, som git ignorerer.
