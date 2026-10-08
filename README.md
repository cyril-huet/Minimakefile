# MiniMake

MiniMake is a small `make`-like program written in C99.

The project is meant to understand the basic ideas behind a build tool:
reading rules, expanding variables, following dependencies and executing
commands.

It is intentionally limited. It is an educational project, not a replacement
for GNU Make.

## Features

- `target: dependency` rules;
- variables with `VAR=value`;
- variable use with `$(VAR)`, `${VAR}` and `$V`;
- recursive variable expansion;
- `$@`, `$<` and `$^` in commands;
- dependency order;
- simple timestamp checks;
- `@` commands without command echo;
- `-f`, `-p` and `-h` options;
- a small shell test suite.

## Build

```sh
make
```

The executable is named `minimake`.

## Usage

Use the first target in a Makefile:

```sh
./minimake
```

Choose another file:

```sh
./minimake -f tests/Makefile-Rules/Makefile1
```

Choose a target explicitly:

```sh
./minimake -f Makefile all
```

Show the selected Makefile name:

```sh
./minimake -p
```

## Example

```make
CC = cc

all: hello

hello:
	echo hello
```

## Project structure

```text
.
├── Makefile
├── src/
│   ├── excecuter_commandes.c
│   ├── lire_fichier.c
│   ├── main.c
│   ├── minimake.h
│   ├── nettoyage.c
│   ├── target.c
│   └── variables.c
└── tests/
    ├── Makefile-Variables/
    ├── Makefile-Rules/
    ├── Makefile-Variables-Special/
    ├── Makefile-Excution/
    └── test.sh
```

## Tests

```sh
make check
```

The tests cover variable expansion, rules, dependencies, automatic variables,
command execution and error handling.

## Limitations

MiniMake does not implement every feature of Make. In particular, it does not
support pattern rules, parallel builds, `.PHONY`, included Makefiles or the
complete GNU Make language.

The code stays deliberately small so that each part can be read and understood
by a student learning C and Unix programming.
