# Beskrivelse

🌐 [English](Description) · **Dansk** · [Bahasa Melayu](Description-ms)

## Formål

Programmet viser, hvor hurtig en CPU er. Hvor hurtig den ser ud til at være,
afhænger af, hvordan programmet er skrevet, så tallet måler CPU'en og koden
sammen, ikke CPU'en alene.

## Hvad det gør

Programmet tester hvert tal fra 0 til 1.000.000 med `is_prime_avx2`, som bruger
AVX2-prøvedivision med 6k ± 1 (fire divisorer pr. trin). Det udskriver hvert
primtal på sin egen linje og skriver en opsummering til standard error:

```text
Found 78498 primes from 0 to 1000000 in 13.806 ms
```

78.498 er det kendte antal primtal op til 1.000.000, så opsummeringen virker
også som kontrol af, at resultatet er rigtigt.

## Hvorfor tiden afhænger af koden

Den samme CPU giver meget forskellige tider for forskellige programmer:

- algoritmen (prøvedivision, en si, en segmenteret si),
- instruktionssættet (skalar kode, AVX2, AVX-512),
- compiler og flag (`-O3 -mavx2`, MSVC `/O2 /arch:AVX2`),
- hvad der måles (kun søgningen, eller også udskrivning til en konsol).

Her måles kun søgningen. Resultatet samles først og udskrives bagefter, så
konsollens hastighed ikke påvirker tallet.
