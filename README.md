# Minimake

Minimake is a simplified implementation of the classic `make` tool from the Unix world, written in C.  
The goal of this project is to understand how build systems work internally by parsing a Makefile and executing rules based on dependencies.

The program reads a Makefile, builds targets, executes commands, and avoids rebuilding files that are already up to date.

## Overview

This project focuses on low-level C programming concepts such as:

- file parsing
- dependency resolution
- process execution (`fork`, `exec`)
- file timestamps (`stat`)
- variable expansion
- build automation logic

The implementation is intentionally minimal and restricted, in order to emphasize correctness, clarity, and understanding of how `make` works under the hood.

## Features

Minimake supports the following:

- Rules:
  - `target: dependencies`
- Variables:
  - `VAR=value`
  - usage: `$(VAR)` / `${VAR}` / `$V`
- Command execution via `/bin/sh -c`
- Dependency checking (up-to-date logic)
- Default target = first rule
- Command logging
- Stop on first error

### Supported options

- `-f file` — specify Makefile
- `-h` — display help
- `-p` — pretty-print parsed Makefile

### Advanced features

- Recursive variables
- Environment variables
- Special variables:
  - `$@` → target name
  - `$<` → first dependency
  - `$^` → all dependencies
- Phony targets (`.PHONY`)
- Pattern rules (`%.o: %.c`)
- Target deduplication

## Constraints

- No global variables
- No `system`, `popen`, `getopt`
- Only standard C library allowed
- Code follows the C99 standard
- Output must strictly match subject format

## Build

Build the project using `make`:

```sh
make
```

## Usage
### Run minimake:
```sh
./minimake
```
### Specify a Makefile:
```sh
./minimake -f Makefile
```
### Run specific targets:
```sh
./minimake all clean
```

## Example
```Makefile
CC = gcc

all: main

main: main.o
	$(CC) -o main main.o

main.o: main.c
	$(CC) -c main.c
```

