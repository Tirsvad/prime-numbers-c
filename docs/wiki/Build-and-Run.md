# Build and run

🌐 **English** · [Dansk](Build-and-Run-da) · [Bahasa Melayu](Build-and-Run-ms)

The `Makefile` is in the repository root and puts all three executables
(`prime_scalar`, `prime_avx2` and `prime_sieve`) in `dist/`.
It needs `gcc` (Linux, or MinGW-w64 / MSYS2 on Windows) and a CPU with AVX2.

```bash
make
```

```bash
make run
```

`make run` runs `prime_scalar`, `prime_avx2` and then `prime_sieve`.

```bash
make clean
```

To save the primes and still see the time, redirect only standard output:

```bash
./dist/prime_sieve > primes.txt
```

## Rust and C#

The same three algorithms are also in Rust and C#, as one program each that
picks the algorithm from its first argument. `make` does not build them.

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

Use `scalar`, `avx2` or `sieve` as the argument. Rust needs `cargo` and C#
needs the .NET SDK (10). Build output goes to `src/rust/target/` and
`src/csharp/bin/`, `src/csharp/obj/` and `src/csharp/bin/out/`; these folders are
ignored by git (`out/` is in the `-o` example above, add it to your own
`.gitignore` if you keep it).
