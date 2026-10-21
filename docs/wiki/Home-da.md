# prime-numbers-c

🌐 [English](Home) · **Dansk** · [Bahasa Melayu](Home-ms)

Tre små C-programmer, der hver udskriver alle primtal fra 0 til 1.000.000 og
viser, hvor lang tid det tog: almindelig prøvedivision (`prime_scalar`),
AVX2-prøvedivision (`prime_avx2`) og Eratosthenes' si (`prime_sieve`).

- [Beskrivelse](Description-da): hvad programmerne skal bruges til, og hvordan de adskiller sig
- [Sådan virker AVX2](How-AVX2-Works-da): koden, trin for trin
- [Resultater](Results-da): målte tider (gennemsnit af 10 kørsler, under WSL)
- [Byg og kør](Build-and-Run-da): `make`, `make run`
