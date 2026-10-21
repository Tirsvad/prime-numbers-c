# Resultater

🌐 [English](Results) · **Dansk** · [Bahasa Melayu](Results-ms)

Tid til at finde alle primtal fra 0 til 1.000.000. Kun søgningen måles.

Hver tid er gennemsnittet af 10 kørsler.

| CPU | Program | Tråde | Resultat |
| --- | --- | --- | --- |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar` | 1 tråd | Found 78498 primes from 0 to 1000000 in 27.675 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar_mt` | 20 tråde | Found 78498 primes from 0 to 1000000 in 4.790 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2` | 1 tråd | Found 78498 primes from 0 to 1000000 in 13.804 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2_mt` | 20 tråde | Found 78498 primes from 0 to 1000000 in 3.814 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_sieve` | 1 tråd | Found 78498 primes from 0 to 1000000 in 1.183 ms |

AVX2 er omkring 2 gange hurtigere end den skalare version. `_mt`-programmerne
kører på alle CPU'ens 20 tråde (se nedenfor). Sien er hurtigst af alle, selv
hurtigere end prøvedivisionen på 20 tråde, fordi den gør langt mindre arbejde.

## Parallelle versioner

`prime_scalar_mt` og `prime_avx2_mt` laver den samme prøvedivision som
`prime_scalar` og `prime_avx2`, men på alle CPU-tråde med OpenMP. Hvert tal
testes for sig, så løkken kan deles: hver tråd tager den næste blok på 1.024 tal,
når den er fri (`schedule(dynamic, 1024)`), fordi større tal koster mere at
teste. Hvert tal skriver sin egen byte i et flag-array, så trådene aldrig rører
den samme hukommelse, og primtallene samles bagefter i rækkefølge på én tråd, så
outputtet er sorteret. Tiden omfatter dette sidste trin. Sien blev ikke gjort
parallel: den tager kun ca. 1 ms, omtrent hvad det koster at starte trådene.

`prime_avx2_mt` er omkring 3,6 gange hurtigere end `prime_avx2`, og
`prime_scalar_mt` omkring 5,8 gange hurtigere end `prime_scalar`, på 20 tråde.
Det er langt fra 20 gange. Sådan falder tiden med antallet af tråde
(`OMP_NUM_THREADS`, passiv ventning, gennemsnit af 10 kørsler):

| Tråde | `prime_scalar_mt` | `prime_avx2_mt` |
| --- | --- | --- |
| 1 | 28.916 ms | 15.069 ms |
| 2 | 16.637 ms | 8.969 ms |
| 4 | 8.251 ms | 4.808 ms |
| 8 | 5.746 ms | 4.014 ms |
| 14 | 4.960 ms | 3.602 ms |
| 20 | 4.910 ms | 3.490 ms |

- Tiden holder op med at falde efter ca. 4 til 8 tråde. En del af arbejdet er
  ikke parallelt (start af trådene, samling af primtallene i rækkefølge), og
  denne CPU har 6 hurtige kerner med 2 tråde hver og 8 langsommere kerner, så de
  sidste tråde giver lidt.
- **Ventepolitik.** Som standard spinder OpenMP-tråde, der løber tør for arbejde,
  og venter. Med alle 20 tråde optaget under WSL gjorde det programmerne
  langsomme og ustabile (mellem 10 og 20 ms i tre målinger, mod 3,5 til 4,9 ms).
  Tabellen ovenfor og hovedtabellen bruger `OMP_WAIT_POLICY=passive`, hvor
  ventende tråde sover. Hvis `_mt`-programmerne er langsommere end de serielle
  på din maskine, så prøv at sætte den:

```bash
OMP_WAIT_POLICY=passive ./dist/prime_avx2_mt
```

## Baggrundstråd og GPU

To programmer mere, målt i samme session som tabellerne ovenfor, ved siden af de
serielle C-programmer og `_mt`-programmerne til sammenligning:

| Program | Gennemsnit af 10 kørsler |
| --- | --- |
| `prime_scalar` | 27.675 ms |
| `prime_avx2` | 13.804 ms |
| `prime_sieve` | 1.183 ms |
| `prime_sieve_thread` (baggrundstråd) | 1.268 ms |
| `prime_scalar_mt` (20 tråde, passiv ventning) | 4.790 ms |
| `prime_avx2_mt` (20 tråde, passiv ventning) | 3.814 ms |
| `prime_cuda` (RTX 3050) | 5.495 ms |

### Si på baggrundstråd (POSIX)

`prime_sieve_thread` (`src/prime_sieve_thread.c`) kører den samme si som
`prime_sieve`, men på en baggrundstråd: hovedtråden starter den med
`pthread_create`, er fri til at lave andet arbejde, og venter her blot på
resultatet med `pthread_join`. Én ekstra tråd gør det samme arbejde som den
serielle si, så den kan ikke være hurtigere; at starte og samle tråden koster ca.
0,09 ms (1,268 ms mod 1,183 ms). Pointen med denne form er at holde et program
responsivt, mens arbejdet kører, ikke at gøre arbejdet hurtigere. Den kræver
POSIX-tråde og bygges med `-pthread`.

### GPU (CUDA)

`prime_cuda` (`src/prime_cuda.cu`) kører prøvedivisionen på et NVIDIA-GPU: én
GPU-tråd tester ét tal, skriver en flag-byte, og CPU'en samler primtallene i
rækkefølge. Det bygges med `nvcc` (`make cuda`) og er ikke en del af `make`.
GPU'en her er et NVIDIA GeForce RTX 3050, nået fra WSL gennem Windows-driveren,
med CUDA 13.3.

- Tiden i tabellen, 5,5 ms, omfatter allokering af GPU-hukommelse, kørsel af
  kernen, kopiering af flagene tilbage og indsamling af primtallene. Selve
  kernen tager ca. 2,5 ms.
- Den omfatter **ikke** den engangsopstart af CUDA-runtime, som tog 130 til
  170 ms i disse kørsler (første CUDA-kald). Programmet udskriver den separat.
- GPU'en er langsommere end CPU-programmerne på 20 tråde (3,8 til 4,8 ms) og end
  sien (1,2 ms) ved denne størrelse. Der er lidt arbejde (1.000.000 tal) til
  tusindvis af GPU-tråde, og kopierings- og opstartsomkostningerne er faste.
  Kernen bremses sandsynligvis også af, at 32 nabotal kører i takt, og en gruppe er
  lige så langsom som dens langsomste tal: et primtal kræver ca. 160 løkketrin,
  mens de fleste sammensatte tal stopper med det samme. Det blev ikke målt. En
  meget større grænse ville favorisere GPU'en, men det blev ikke prøvet.

## Andre sprog

De samme algoritmer er også skrevet i Rust (`src/rust/`) og C#
(`src/csharp/`). De beholdes som dokumentation, så koden i de tre sprog kan
læses side om side. Hver udskriver de samme 78.498 primtal som C-programmerne.
Køretid, gennemsnit af 10 kørsler under WSL på samme CPU; `-mt` er den
parallelle version på alle 20 tråde:

| Sprog | `scalar` | `scalar-mt` | `avx2` | `avx2-mt` | `sieve` |
| --- | --- | --- | --- | --- | --- |
| C (gcc 12.2) | 27.675 ms | 4.790 ms | 13.804 ms | 3.814 ms | 1.183 ms |
| Rust 1.99.1 | 29.020 ms | 5.413 ms | 14.580 ms | 4.415 ms | 1.294 ms |
| C# (.NET 10) | 48.883 ms | 18.855 ms | 16.704 ms | 17.894 ms | 1.850 ms |

Rækkefølgen er den samme i alle tre sprog: sien er klart hurtigst, og AVX2 er
hurtigere end skalar. C og Rust ligger tæt; da hele målingen blev gentaget en
anden gang, flyttede gennemsnittene sig op til ca. 20 % (fx C skalar mellem 25
og 29 ms), så en forskel af den størrelse skal ikke læses som reel. Programmerne
adskiller sig i mere end sproget (compiler, runtime og biblioteksdetaljer), så
læs tabellen som en grov sammenligning.

- **Rust** 1.99.1, `cargo build --release` (`opt-level = 3`). AVX2-versionen
  bruger `std::arch`-intrinsics med `#[target_feature(enable = "avx2")]`.
  `-mt`-versionerne bruger `std::thread::scope` med en fælles bloktæller og
  ingen ekstra crates.
- **C#** på .NET 10, en Release-build med trinvis kompilering slået fra, så JIT
  straks laver fuldt optimeret kode. AVX2-versionen bruger
  `System.Runtime.Intrinsics`. `-mt`-versionerne bruger `Parallel.For`.
  JIT-tiden for den målte kode indgår i den målte tid, og opstarten af
  .NET-runtime gør ikke.
- **C# er langsom i `-mt`-tilstandene** (ca. 18 ms, ikke bedre end seriel AVX2).
  Det første `Parallel.For` i en proces betaler en opstartspris på ca. 14 ms
  (trådpulje og biblioteksopstart). Samme kald igen i samme proces tog 3 til
  4 ms, lige så hurtigt som Rust, og det samme gjorde en løkke med almindelige
  tråde startet i hånden. Det blev kontrolleret i et engangsforsøg. Et program,
  der kun har ét kort parallelt job som her, betaler den pris hver gang.

### Hvorfor divisionens bredde betyder noget for det skalare program

Det skalare program bruger sin tid på divisionen `n % d`. I en første version af
C-programmet var `d` en `uint64_t`, og gcc lavede `n % d` om til en 64-bit
division (`divq`). Den version tog ca. 46,7 ms, mens Rust-programmet tog ca.
27 ms i den samme sammenligning. Årsagen er, at Rust-compileren (LLVM) kunne se,
at `n` og `d` kan være i 32 bit, og brugte den billigere 32-bit division
(`divl`); på denne CPU er en 64-bit division langsommere end en 32-bit.

`prime_scalar.c` erklærer nu `d` som `uint32_t` (kun `(uint64_t)d * d` regnes i
64 bit, så grænsen ikke kan løbe over). C er så lige så hurtig som Rust.
Tabellen viser denne version.

## Byggetid og filstørrelse

Byggetid er gennemsnittet af 5 rene byg (build-output slettet med `rm -rf` før
hvert) under WSL. Filstørrelsen er størrelsen på den fil, der bygges.

| | Byggetid | Filstørrelse | Strippet |
| --- | --- | --- | --- |
| C `prime_scalar` (gcc `-O3`) | 207 ms | 16.136 bytes | 14.496 bytes |
| C `prime_scalar_mt` (`-O3 -fopenmp`) | 189 ms | 16.720 bytes | 14.528 bytes |
| C `prime_avx2` (`-O3 -mavx2`) | 416 ms | 16.216 bytes | 14.504 bytes |
| C `prime_avx2_mt` (`-O3 -mavx2 -fopenmp`) | 396 ms | 16.808 bytes | 14.536 bytes |
| C `prime_sieve` (`-O3 -mavx2`) | 237 ms | 16.224 bytes | 14.504 bytes |
| C alle fem (sum af de fem rækker) | 1.445 ms | 82.104 bytes | |
| C `prime_sieve_thread` (`-O3 -pthread`) | 206 ms | 16.432 bytes | 14.528 bytes |
| C `prime_cuda` (`nvcc -O3 -arch=native`) | 820 ms | 1.087.000 bytes | 769.904 bytes |
| Rust `cargo build --release` (alle fem) | 521 ms | 563.504 bytes | 428.184 bytes |
| C# `dotnet build -c Release` (alle fem) | 1.069 ms | `prime.dll` 8.704 bytes | |

- **`prime_cuda` er større** (ca. 1 MB), fordi CUDA-runtime er linket statisk ind,
  og den indeholder GPU-koden til denne maskines GPU. Den kræver NVIDIA-driveren
  for at køre.
- **Ét program eller fem.** C bygger fem programmer. Rust- og C#-programmet
  indeholder alle fem tilstande og vælger én ud fra det første argument
  (`prime scalar|scalar-mt|avx2|avx2-mt|sieve`), så ét byg dækker dem alle.
  Rækkerne "alle fem" er dem, man skal sammenligne.
- **Rust er større**, fordi standardbiblioteket er linket ind i programmet (ca.
  428 KB efter `strip`). C-programmerne bruger systemets C-bibliotek, som ikke
  tæller med i deres størrelse. `-fopenmp`-programmerne linker OpenMP-biblioteket
  (`libgomp`) dynamisk, så det tæller heller ikke med.
- **C#** bygger en lille `prime.dll` (8.704 bytes; 104.339 bytes for hele
  output-mappen med `prime.pdb` og `.json`-filerne). Den indeholder ikke
  .NET-runtime, som skal være installeret for at køre den. Det første C#-byg var
  det langsomste (1.974 ms mod ca. 850 ms for de andre fire), fordi byggeserveren
  starter dér.
- **Byggetiden gælder** et program uden andre afhængigheder. Et større program,
  et inkrementelt byg eller en kold cache giver andre tal.

## Sådan blev resultaterne målt

C-programmerne blev bygget med `make` og Rust- og C#-programmerne med
kommandoerne i [Byg og kør](Build-and-Run-da), og alt blev kørt under WSL 2
(Windows Subsystem for Linux) på Windows 11: Debian med gcc 12.2 og Linux-kerne
6.18. WSL kører en rigtig Linux-kerne i en let virtuel maskine på den samme 13th
Gen Intel(R) Core(TM) i5-13600K (20 tråde), så CPU'en er den samme som på
værten. Compilerflag: `-O3 -mavx2` for `prime_avx2` og `prime_sieve`, `-O3` uden
`-mavx2` for `prime_scalar`, så compileren ikke kan bruge AVX2 dér, og
`-fopenmp` for `_mt`-programmerne. Hvert program blev startet 10 gange, og
tabellerne viser gennemsnittet. Tiderne varierer nogle få millisekunder fra
kørsel til kørsel, og alle kørsler er med i gennemsnittet.

Tider under WSL kan afvige en smule fra native Linux eller Windows, så
sammenlign kun tal, der er målt på samme måde.

Et resultat kan kun sammenlignes med et andet, når koden, compileren og flagene
er de samme. Sådan tilføjer du et resultat: kør programmerne på din maskine (se
[Byg og kør](Build-and-Run-da)) og tilføj en række pr. program med dit CPU-navn,
antallet af tråde og opsummeringslinjen.
