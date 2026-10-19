# Builds prime_avx2 into dist/ on Linux (gcc) and Windows (MinGW-w64 / MSYS2 gcc).
#   make          build
#   make run      build and run
#   make clean    remove dist/

CC      ?= gcc
CFLAGS  ?= -O3 -mavx2 -Wall -Wextra
LDLIBS  += -lm

DIST    := dist

ifeq ($(OS),Windows_NT)
  EXE   := .exe
  # make on Windows may use cmd.exe, which needs backslashes
  RUN   := $(DIST)\prime_avx2$(EXE)
  RMDIR := if exist $(DIST) rmdir /S /Q $(DIST)
else
  EXE   :=
  RUN   := ./$(DIST)/prime_avx2
  RMDIR := rm -rf $(DIST)
endif

TARGET := $(DIST)/prime_avx2$(EXE)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): src/prime_avx2.c | $(DIST)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

$(DIST):
	mkdir $(DIST)

run: $(TARGET)
	$(RUN)

clean:
	-$(RMDIR)
