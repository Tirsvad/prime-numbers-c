# Builds three programs into dist/ on Linux (gcc) and Windows (MinGW-w64 / MSYS2 gcc):
#   prime_avx2    AVX2 trial division
#   prime_scalar  plain scalar trial division (built without -mavx2)
#   prime_sieve   sieve of Eratosthenes
#   make          build all three
#   make run      build and run all three, one after the other
#   make clean    remove dist/

CC      ?= gcc
CFLAGS  ?= -O3 -mavx2 -Wall -Wextra
LDLIBS  += -lm

DIST    := dist

ifeq ($(OS),Windows_NT)
  EXE   := .exe
  # make on Windows may use cmd.exe, which needs backslashes
  RUN_AVX2  := $(DIST)\prime_avx2$(EXE)
  RUN_SCALAR := $(DIST)\prime_scalar$(EXE)
  RUN_SIEVE := $(DIST)\prime_sieve$(EXE)
  RMDIR := if exist $(DIST) rmdir /S /Q $(DIST)
else
  EXE   :=
  RUN_AVX2  := ./$(DIST)/prime_avx2
  RUN_SCALAR := ./$(DIST)/prime_scalar
  RUN_SIEVE := ./$(DIST)/prime_sieve
  RMDIR := rm -rf $(DIST)
endif

TARGETS := $(DIST)/prime_avx2$(EXE) $(DIST)/prime_scalar$(EXE) $(DIST)/prime_sieve$(EXE)

.PHONY: all run clean

all: $(TARGETS)

$(DIST)/%$(EXE): src/%.c | $(DIST)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

# The scalar program is built without AVX2 so the compiler cannot vectorise it.
$(DIST)/prime_scalar$(EXE): CFLAGS = -O3 -Wall -Wextra

$(DIST):
	mkdir $(DIST)

run: $(TARGETS)
	$(RUN_AVX2)
	$(RUN_SCALAR)
	$(RUN_SIEVE)

clean:
	-$(RMDIR)
