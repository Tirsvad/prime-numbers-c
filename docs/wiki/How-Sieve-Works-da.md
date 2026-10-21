# Sådan virker sien

🌐 [English](How-Sieve-Works) · **Dansk** · [Bahasa Melayu](How-Sieve-Works-ms)

`prime_sieve` (`src/prime_sieve.c`) tester ikke tal et ad gangen. Det skriver
alle tal op på forhånd og streger dem ud, der ikke kan være primtal. Det, der er
tilbage, er primtallene. Det er Eratosthenes' si.

## Idéen

1. Skriv tallene op fra 2 til grænsen.
2. Tag det mindste tal, der ikke er strøget ud. Det er et primtal.
3. Stryg dets multipla ud. De har det som faktor, så de er ikke primtal.
4. Gentag fra trin 2 med det næste tal, der ikke er strøget ud.

To fakta gør, at programmet skal gøre langt mindre:

- **Start ved p × p.** Et multiplum af `p` under `p × p`, fx `3p` eller `5p`, har
  også en mindre primfaktor, så et tidligere primtal har allerede strøget det
  ud. Det første nye er `p × p`.
- **Stop ved sqrt(grænsen).** Ethvert sammensat tal op til grænsen har en
  primfaktor på højst kvadratroden, så primtal over sqrt(1.000.000) = 1.000 har
  intet tilbage at stryge ud.

## Et lille eksempel: primtal op til 50

Kun ulige tal gemmes (se nedenfor), så start med 3, 5, 7, ..., 49:

| Primtal p | Starter ved p × p | Strøget ud |
| --- | --- | --- |
| 3 | 9 | 9, 15, 21, 27, 33, 39, 45 |
| 5 | 25 | 25, 35, 45 |
| 7 | 49 | 49 |
| 9 | 81, over 50 | stop: 9 × 9 er forbi grænsen |

Ulige multipla af `p` ligger `2p` fra hinanden (`p × p`, `p × p + 2p`, ...), så
det er skridtet. Tilbage er 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47.
Med 2 lagt til i hånden er det de 15 primtal op til 50.

## Kun ulige tal, ét bit hver

To valg holder data småt, så det bliver i CPU'ens cache:

- **Kun ulige tal.** Ethvert lige tal over 2 er sammensat, så programmet springer
  dem over og lægger 2 til i hånden. Bit `i` står for det ulige tal `2i + 1`, så
  bit 0 er 1, bit 1 er 3, bit 2 er 5 osv.
- **Ét bit pr. tal.** Et sat bit betyder "sammensat". Bittene er pakket 64 i en
  `uint64_t`. Op til 1.000.000 er det 500.001 bit, ca. 7.800 ord eller 62 KB. Et
  almindeligt array med én byte pr. tal ville fylde 1 MB.

For at nå bit `i` er ordet `i >> 6` (altså `i / 64`), og bitten i det er
`i & 63` (altså `i % 64`).

## Koden, trin for trin

```c
#define SIEVE_BITS (LIMIT / 2 + 1)
#define SIEVE_WORDS ((SIEVE_BITS + 63) / 64)
```

Antallet af bit og antallet af 64-bit-ord, der rummer dem (rundet op).

```c
static uint64_t composite[SIEVE_WORDS];
memset(composite, 0, sizeof composite);
```

Alle bit starter på 0: intet er strøget ud endnu.

```c
for (uint64_t p = 3; p * p <= LIMIT; p += 2) {
```

Prøv hvert ulige `p` fra 3, og stop, når `p × p` passerer grænsen. `p × p`
beregnes i 64 bit, så det ikke kan løbe over.

```c
    if (composite[(p >> 1) >> 6] >> ((p >> 1) & 63) & 1) continue;
```

Bitten for `p` er `p >> 1` (da `p = 2i + 1`, er `i = p >> 1`). Hvis den er sat,
er `p` strøget ud af et mindre primtal, så det er ikke et primtal og springes
over.

```c
    for (uint64_t m = (p * p) >> 1; m < SIEVE_BITS; m += p)
        composite[m >> 6] |= 1ull << (m & 63);
```

Stryg de ulige multipla af `p` ud. Det første er `p × p`, hvis bit er
`(p × p) >> 1`. Det næste ulige multiplum ligger `2p` højere i værdi, altså `p`
højere i bitindeks, så `m += p`. `|=` sætter bitten og lader de andre i ordet
være.

```c
if (LIMIT >= 2) out[count++] = 2;
for (uint32_t i = 1; i < SIEVE_BITS; i++) {
    uint32_t n = 2 * i + 1;
    if (n > LIMIT) break;
    if (!(composite[i >> 6] >> (i & 63) & 1)) out[count++] = n;
}
```

Saml resultatet: først 2, så hvert ulige `n = 2i + 1`, hvis bit stadig er 0. Den
sidste bit kan være et tal forbi grænsen, så løkken stopper ved `n > LIMIT`.

## Hvorfor den er hurtigere

`prime_avx2` og `prime_scalar` tester hvert tal for sig: et primtal nær
1.000.000 kræver ca. 167 løkketrin (divisorerne 6k ± 1 op til 1.000). Sien
dividerer aldrig. Hvert sammensat tal strøges ud med nogle få simple bitskrivninger,
under en million i alt for 1.000.000 tal, og arrayet på 62 KB er i CPU'ens cache.
Derfor er den omkring 12 gange hurtigere end AVX2-versionen og omkring 23 gange
hurtigere end den skalare (se [Resultater](Results-da)).

Sien passer kun til et helt interval. Til at tjekke ét tal er `is_prime_avx2`
det bedre værktøj, og en større grænse giver et større array.
