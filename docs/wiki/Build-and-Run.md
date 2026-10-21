# Build and run

🌐 **English** · [Dansk](Build-and-Run-da) · [Bahasa Melayu](Build-and-Run-ms)

The `Makefile` is in the repository root and puts all six executables
(`prime_scalar`, `prime_scalar_mt`, `prime_avx2`, `prime_avx2_mt`, `prime_sieve`
and `prime_sieve_thread`) in `dist/`.
It needs `gcc` (Linux, or MinGW-w64 / MSYS2 on Windows) and a CPU with AVX2.

```bash
make
```

```bash
make run
```

`make run` runs all six, one after the other. The `_mt` programs use OpenMP
(`-fopenmp`) and run on all CPU threads; `OMP_NUM_THREADS=4` sets another number.
If they are slower than the serial programs on your machine, set
`OMP_WAIT_POLICY=passive` (see [Results](Results), "Parallel versions").

```bash
make clean
```

To save the primes and still see the time, redirect only standard output:

```bash
./dist/prime_sieve > primes.txt
```

## Rust and C#

The same algorithms are also in Rust and C#, as one program each that picks the
algorithm from its first argument. `make` does not build them.

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

Use `scalar`, `scalar-mt`, `avx2`, `avx2-mt` or `sieve` as the argument. Rust needs `cargo` and C#
needs the .NET SDK (10). Build output goes to `src/rust/target/` and
`src/csharp/bin/`, `src/csharp/obj/` and `src/csharp/bin/out/`; these folders are
ignored by git (`out/` is in the `-o` example above, add it to your own
`.gitignore` if you keep it).

## GPU (CUDA)

`prime_cuda` runs on an NVIDIA GPU. It needs the NVIDIA driver and the CUDA
toolkit (`nvcc`), so it is not part of `make`. Build and run it with:

```bash
make cuda
```

```bash
./dist/prime_cuda
```

If `nvcc` is not on your `PATH`, give its location, for example
`make cuda NVCC=/usr/local/cuda-13.3/bin/nvcc`. The build uses `-arch=native`,
which builds for the GPU in the machine.
