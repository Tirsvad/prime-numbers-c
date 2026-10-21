# Resultater

🌐 [English](Results) · **Dansk** · [Bahasa Melayu](Results-ms)

Tid til at finde alle primtal fra 0 til 1.000.000. Kun søgningen måles.

Hver tid er gennemsnittet af 10 kørsler.

| CPU | Program | Resultat |
| --- | --- | --- |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar` | Found 78498 primes from 0 to 1000000 in 46.692 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2` | Found 78498 primes from 0 to 1000000 in 15.841 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_sieve` | Found 78498 primes from 0 to 1000000 in 1.191 ms |

AVX2 er omkring 3 gange hurtigere end den skalare version, og sien er omkring
13 gange hurtigere end AVX2 (omkring 39 gange hurtigere end skalar).

## Sådan blev resultaterne målt

Programmerne blev bygget med `make` og kørt under WSL 2 (Windows Subsystem for
Linux) på Windows 11: Debian med gcc 12.2 og Linux-kerne 6.18. WSL kører en
rigtig Linux-kerne i en let virtuel maskine på den samme 13th Gen Intel(R)
Core(TM) i5-13600K, så CPU'en er den samme som på værten. Compilerflag:
`-O3 -mavx2` for `prime_avx2` og `prime_sieve`, og `-O3` uden `-mavx2` for
`prime_scalar`, så compileren ikke kan bruge AVX2 dér. Hvert program blev startet
10 gange, og tabellen viser gennemsnittet. Tiderne varierer
nogle få millisekunder fra kørsel til kørsel, og alle kørsler er med i
gennemsnittet.

Tider under WSL kan afvige en smule fra native Linux eller Windows, så
sammenlign kun tal, der er målt på samme måde.

Et resultat kan kun sammenlignes med et andet, når koden, compileren og flagene
er de samme. Sådan tilføjer du et resultat: kør programmet på din maskine (se
[Byg og kør](Build-and-Run-da)) og tilføj en række med dit CPU-navn og
opsummeringslinjen.
