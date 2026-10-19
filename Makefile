# Builds prime_avx2 on Linux (gcc) and Windows (MinGW-w64 / MSYS2 gcc).
#   make          build
#   make run      build and run
#   make clean    remove build output

CC      ?= gcc
CFLAGS  ?= -O3 -mavx2 -Wall -Wextra
LDLIBS  += -lm

ifeq ($(OS),Windows_NT)
  EXE   := .exe
  RM    := del /Q /F
  # make on Windows may use cmd.exe, which needs backslashes
  RUN   := .\prime_avx2$(EXE)
  RMTARGET := prime_avx2$(EXE)
else
  EXE   :=
  RM    := rm -f
  RUN   := ./prime_avx2
  RMTARGET := prime_avx2
endif

TARGET := prime_avx2$(EXE)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): prime_avx2.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

run: $(TARGET)
	$(RUN)

clean:
	-$(RM) $(RMTARGET)
