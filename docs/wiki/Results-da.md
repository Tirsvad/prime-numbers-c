# Resultater

🌐 [English](Results) · **Dansk** · [Bahasa Melayu](Results-ms)

Tid til at finde alle primtal fra 0 til 1.000.000. Kun søgningen måles.

Hver tid er gennemsnittet af 10 kørsler.

| CPU | Program | Resultat |
| --- | --- | --- |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_scalar` | Found 78498 primes from 0 to 1000000 in 25.330 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_avx2` | Found 78498 primes from 0 to 1000000 in 12.856 ms |
| 13th Gen Intel(R) Core(TM) i5-13600K | `prime_sieve` | Found 78498 primes from 0 to 1000000 in 1.103 ms |

AVX2 er omkring 2 gange hurtigere end den skalare version, og sien er omkring
12 gange hurtigere end AVX2 (omkring 23 gange hurtigere end skalar).

## Andre sprog

De samme tre algoritmer er også skrevet i Rust (`src/rust/`) og C#
(`src/csharp/`). De beholdes som dokumentation, så koden i de tre sprog kan
læses side om side. Hver udskriver de samme 78.498 primtal som C-programmerne.
Køretid, gennemsnit af 10 kørsler under WSL på samme CPU:

| Sprog | `scalar` | `avx2` | `sieve` |
| --- | --- | --- | --- |
| C (gcc 12.2) | 25.330 ms | 12.856 ms | 1.103 ms |
| Rust 1.99.1 | 30.580 ms | 16.358 ms | 1.439 ms |
| C# (.NET 10) | 51.949 ms | 18.243 ms | 1.911 ms |

Rækkefølgen er den samme i alle tre sprog: sien er klart hurtigst, så AVX2, så
skalar. C# er langsommere i det skalare program og lidt langsommere i sien.
Forskellene mellem C og Rust er små, og når hele målingen blev gentaget en anden
gang, flyttede gennemsnittene sig op til ca. 20 % (fx C skalar mellem 25 og
29 ms), så en forskel af den størrelse skal ikke læses som reel. Programmerne
adskiller sig i mere end sproget (compiler, runtime og biblioteksdetaljer), så
læs tabellen som en grov sammenligning.

- **Rust** 1.99.1, `cargo build --release` (`opt-level = 3`). AVX2-versionen
  bruger `std::arch`-intrinsics med `#[target_feature(enable = "avx2")]`.
- **C#** på .NET 10, en Release-build med trinvis kompilering slået fra, så JIT
  straks laver fuldt optimeret kode. AVX2-versionen bruger
  `System.Runtime.Intrinsics`. JIT-tiden for den målte kode indgår i den målte
  tid, og opstarten af .NET-runtime gør ikke.

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
| C `prime_avx2` (gcc `-O3 -mavx2`) | 416 ms | 16.216 bytes | 14.504 bytes |
| C `prime_sieve` (gcc `-O3 -mavx2`) | 237 ms | 16.224 bytes | 14.504 bytes |
| C alle tre med `make` | 1.144 ms | 48.576 bytes | |
| Rust `cargo build --release` (alle tre) | 397 ms | 491.824 bytes | 376.288 bytes |
| C# `dotnet build -c Release` (alle tre) | 990 ms | `prime.dll` 7.680 bytes | |

- **Ét program eller tre.** C bygger tre programmer. Rust- og C#-programmet
  indeholder alle tre algoritmer og vælger én ud fra det første argument
  (`prime scalar|avx2|sieve`), så ét byg dækker alle tre. Rækkerne "alle tre" er
  dem, man skal sammenligne.
- **Rust er større**, fordi standardbiblioteket er linket ind i programmet (ca.
  376 KB efter `strip`). C-programmerne bruger systemets C-bibliotek, som ikke
  tæller med i deres størrelse.
- **C#** bygger en lille `prime.dll` (7.680 bytes; 102.847 bytes for hele
  output-mappen med `prime.pdb` og `.json`-filerne). Den indeholder ikke
  .NET-runtime, som skal være installeret for at køre den. Det første C#-byg var
  det langsomste (1.732 ms mod ca. 800 ms for de andre fire), fordi byggeserveren
  starter dér.
- **Byggetiden gælder** et program uden andre afhængigheder. Et større program,
  et inkrementelt byg eller en kold cache giver andre tal.

## Sådan blev resultaterne målt

C-programmerne blev bygget med `make` og Rust- og C#-programmerne med
kommandoerne i [Byg og kør](Build-and-Run-da), og alt blev kørt under WSL 2 (Windows Subsystem for Linux)
på Windows 11: Debian med gcc 12.2 og Linux-kerne 6.18. WSL kører en rigtig
Linux-kerne i en let virtuel maskine på den samme 13th Gen Intel(R) Core(TM)
i5-13600K, så CPU'en er den samme som på værten. Compilerflag: `-O3 -mavx2` for
`prime_avx2` og `prime_sieve`, og `-O3` uden `-mavx2` for `prime_scalar`, så
compileren ikke kan bruge AVX2 dér. Hvert program blev startet 10 gange, og
tabellerne viser gennemsnittet. Tiderne varierer nogle få millisekunder fra
kørsel til kørsel, og alle kørsler er med i gennemsnittet.

Tider under WSL kan afvige en smule fra native Linux eller Windows, så
sammenlign kun tal, der er målt på samme måde.

Et resultat kan kun sammenlignes med et andet, når koden, compileren og flagene
er de samme. Sådan tilføjer du et resultat: kør programmerne på din maskine (se
[Byg og kør](Build-and-Run-da)) og tilføj en række pr. program med dit CPU-navn
og opsummeringslinjen.
