# prime-numbers-c

🌐 [English](Home) · **Dansk** · [Bahasa Melayu](Home-ms)

Små C-programmer, der hver udskriver alle primtal fra 0 til 1.000.000 og viser,
hvor lang tid det tog: almindelig prøvedivision (`prime_scalar`),
AVX2-prøvedivision (`prime_avx2`), begge også på alle CPU-tråde (`_mt`), og
Eratosthenes' si (`prime_sieve`).

- [Beskrivelse](Description-da): hvad programmerne skal bruges til, og hvordan de adskiller sig
- [Sådan virker AVX2](How-AVX2-Works-da): AVX2-koden, trin for trin
- [Sådan virker sien](How-Sieve-Works-da): sikoden, trin for trin
- [Resultater](Results-da): målte tider (gennemsnit af 10 kørsler, under WSL)
- [Byg og kør](Build-and-Run-da): `make`, `make run`
