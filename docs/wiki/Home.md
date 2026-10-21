# prime-numbers-c

🌐 **English** · [Dansk](Home-da) · [Bahasa Melayu](Home-ms)

Three small C programs that each list every prime number from 0 to 1,000,000
and report how long it took: plain trial division (`prime_scalar`), AVX2 trial
division (`prime_avx2`) and a sieve of Eratosthenes (`prime_sieve`).

- [Description](Description): what the programs are for and how they differ
- [How AVX2 works](How-AVX2-Works): the code, step by step
- [Results](Results): measured times (average of 10 runs, under WSL)
- [Build and run](Build-and-Run): `make`, `make run`
