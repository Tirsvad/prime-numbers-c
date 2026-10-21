# Builds five programs into dist/ on Linux (gcc) and Windows (MinGW-w64 / MSYS2 gcc):
#   prime_avx2       AVX2 trial division
#   prime_avx2_mt    the same on all CPU threads (OpenMP)
#   prime_scalar     plain scalar trial division (built without -mavx2)
#   prime_scalar_mt  the same on all CPU threads (OpenMP)
#   prime_sieve      sieve of Eratosthenes
#   make          build all five
#   make run      build and run all five, one after the other
#   make clean    remove dist/

CC      ?= gcc
CFLAGS  ?= -O3 -mavx2 -Wall -Wextra
LDLIBS  += -lm

DIST    := dist

ifeq ($(OS),Windows_NT)
  EXE   := .exe
  # make on Windows may use cmd.exe, which needs backslashes
  RUN_AVX2  := $(DIST)\prime_avx2$(EXE)
  RUN_AVX2_MT := $(DIST)\prime_avx2_mt$(EXE)
  RUN_SCALAR := $(DIST)\prime_scalar$(EXE)
  RUN_SCALAR_MT := $(DIST)\prime_scalar_mt$(EXE)
  RUN_SIEVE := $(DIST)\prime_sieve$(EXE)
  RMDIR := if exist $(DIST) rmdir /S /Q $(DIST)
else
  EXE   :=
  RUN_AVX2  := ./$(DIST)/prime_avx2
  RUN_AVX2_MT := ./$(DIST)/prime_avx2_mt
  RUN_SCALAR := ./$(DIST)/prime_scalar
  RUN_SCALAR_MT := ./$(DIST)/prime_scalar_mt
  RUN_SIEVE := ./$(DIST)/prime_sieve
  RMDIR := rm -rf $(DIST)
endif

TARGETS := $(DIST)/prime_avx2$(EXE) $(DIST)/prime_avx2_mt$(EXE) \
           $(DIST)/prime_scalar$(EXE) $(DIST)/prime_scalar_mt$(EXE) \
           $(DIST)/prime_sieve$(EXE)

.PHONY: all run clean

all: $(TARGETS)

$(DIST)/%$(EXE): src/%.c | $(DIST)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

# The scalar program is built without AVX2 so the compiler cannot vectorise it.
$(DIST)/prime_scalar$(EXE): CFLAGS = -O3 -Wall -Wextra

# The _mt programs run on all CPU threads with OpenMP.
$(DIST)/prime_avx2_mt$(EXE): CFLAGS = -O3 -mavx2 -fopenmp -Wall -Wextra
$(DIST)/prime_scalar_mt$(EXE): CFLAGS = -O3 -fopenmp -Wall -Wextra

$(DIST):
	mkdir $(DIST)

run: $(TARGETS)
	$(RUN_SCALAR)
	$(RUN_SCALAR_MT)
	$(RUN_AVX2)
	$(RUN_AVX2_MT)
	$(RUN_SIEVE)

clean:
	-$(RMDIR)
