# Beskrivelse

🌐 [English](Description) · **Dansk** · [Bahasa Melayu](Description-ms)

## Formål

Programmet viser, hvor hurtig en CPU er. Hvor hurtig den ser ud til at være,
afhænger af, hvordan programmet er skrevet, så tallet måler CPU'en og koden
sammen, ikke CPU'en alene.

## Hvad det gør

Programmet finder alle primtal fra 0 til 1.000.000 på flere måder, i separate
programmer, så koden er nem at sammenligne:

- `src/prime_scalar.c` bygger `prime_scalar`. Det tester hvert tal med almindelig
  prøvedivision med 6k ± 1, én divisor pr. trin, uden AVX2.
- `src/prime_avx2.c` bygger `prime_avx2`. Det tester hvert tal med
  `is_prime_avx2`, den samme prøvedivision med AVX2 (fire divisorer pr. trin).
  Se [Sådan virker AVX2](How-AVX2-Works-da).
- `src/prime_scalar_mt.c` og `src/prime_avx2_mt.c` bygger `prime_scalar_mt` og
  `prime_avx2_mt`: de samme to programmer, hvor tallene er delt ud på alle
  CPU-tråde med OpenMP. Se [Resultater](Results-da) ("Parallelle versioner").
- `src/prime_sieve.c` bygger `prime_sieve`. Det bruger Eratosthenes' si over
  ulige tal, gemt som et bitfelt (ca. 62 KB, så det bliver i CPU'ens cache). Se
  [Sådan virker sien](How-Sieve-Works-da).
- `src/prime_sieve_thread.c` bygger `prime_sieve_thread`: den samme si på en
  POSIX-baggrundstråd. `src/prime_cuda.cu` bygger `prime_cuda`: prøvedivisionen på et
  NVIDIA-GPU. Se [Resultater](Results-da) ("Baggrundstråd og GPU").

Alle udskriver hvert primtal på sin egen linje og skriver en opsummering til
standard error (`_mt`-programmerne starter den med antallet af tråde):

```text
prime_scalar: Found 78498 primes from 0 to 1000000 in 27.675 ms (gennemsnit af 10 kørsler)
prime_scalar_mt: [20 threads] Found 78498 primes from 0 to 1000000 in 4.790 ms (gennemsnit af 10 kørsler)
prime_avx2: Found 78498 primes from 0 to 1000000 in 13.804 ms (gennemsnit af 10 kørsler)
prime_avx2_mt: [20 threads] Found 78498 primes from 0 to 1000000 in 3.814 ms (gennemsnit af 10 kørsler)
prime_sieve: Found 78498 primes from 0 to 1000000 in 1.183 ms (gennemsnit af 10 kørsler)
```

De samme algoritmer er også skrevet i Rust (`src/rust/`) og C# (`src/csharp/`) til
sammenligning; se [Resultater](Results-da) for deres tider, byggetider og filstørrelser.

Tiderne er gennemsnittet af 10 kørsler under WSL (se [Resultater](Results-da)).
AVX2 gør prøvedivisionen omkring 2 gange hurtigere end den skalare version, og
trådene gør hver af dem 3,6 til 5,8 gange hurtigere igen. Den serielle si er
stadig hurtigere end dem alle, fordi den undgår at teste hvert tal for sig. Alle
fem udskriver de samme primtal.

## Hvorfor tiden afhænger af koden

Den samme CPU giver meget forskellige tider for forskellige programmer:

- algoritmen (prøvedivision, en si, en segmenteret si),
- instruktionssættet (skalar kode, AVX2, AVX-512),
- compiler og flag (`-O3 -mavx2`, MSVC `/O2 /arch:AVX2`),
- hvad der måles (kun søgningen, eller også udskrivning til en konsol).

Her måles kun søgningen. Resultatet samles først og udskrives bagefter, så
konsollens hastighed ikke påvirker tallet.
