# prime-numbers-c

Lists every prime number from 0 to 1,000,000 and reports how long the search
took. It shows how fast a CPU is, but the result depends on how the program is
written: algorithm, instruction set (here AVX2), compiler and flags.

Example result on a 13th Gen Intel(R) Core(TM) i5-13600K:

```text
Found 78498 primes from 0 to 1000000 in 20.977 ms
```

## Build and run

Needs `gcc` (Linux, or MinGW-w64 / MSYS2 on Windows), `make` and a CPU with AVX2.

```bash
make
```

```bash
make run
```

```bash
make clean
```

The executable is built into `dist/`. The primes go to standard output and the
timing line to standard error, so this saves the list and still shows the time:

```bash
./dist/prime_avx2 > primes.txt
```

## Layout

| Path | Content |
| --- | --- |
| `src/prime_avx2.c` | the program |
| `Makefile` | build rules |
| `docs/wiki/` | wiki pages, published to the Gitea wiki by `.gitea/workflows/publish-wiki.yml` |

## Documentation

See the wiki: Description, Results and Build and run.

## License

See [LICENSE](LICENSE).
