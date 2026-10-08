# MiniMakefile

[![CI](https://github.com/cyril-huet/Minimakefile/actions/workflows/ci.yml/badge.svg)](https://github.com/cyril-huet/Minimakefile/actions/workflows/ci.yml)
![C](https://img.shields.io/badge/C-C99-blue)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)

MiniMakefile is a small `make`-like build tool written in C99.

The project was created to understand how a build tool works internally:

- reading a Makefile;
- parsing targets and dependencies;
- expanding variables;
- checking file timestamps;
- executing shell commands.

MiniMakefile is intentionally limited. It is an educational project and is not intended to replace GNU Make.

## Features

- Makefile parsing;
- targets and dependencies;
- variables with `NAME = value`;
- variable expansion with `$(NAME)`, `${NAME}` and `$N`;
- recursive variable expansion;
- automatic variables:
  - `$@` for the current target;
  - `$<` for the first dependency;
  - `$^` for all dependencies;
- dependency order;
- basic timestamp checking;
- silent commands with `@`;
- custom Makefile with `-f`;
- Makefile name display with `-p`;
- help with `-h`;
- shell test suite;
- GitHub Actions continuous integration.

## Requirements

- A C99 compiler such as GCC or Clang;
- GNU Make;
- a POSIX-compatible shell;
- `clang-format` for formatting checks.

## Build

Clone the repository and compile the project:

```sh
git clone https://github.com/cyril-huet/Minimakefile.git
cd Minimakefile

make
```

The executable is created at:

```text
./minimake
```

Display the available options:

```sh
./minimake -h
```

## Usage

### Use the default Makefile

```sh
./minimake
```

MiniMakefile first looks for a file named `makefile`, then for a file named `Makefile`.

The first target found in the file is built by default.

### Choose a Makefile

```sh
./minimake -f tests/Makefile-Rules/Makefile1
```

### Choose a target

```sh
./minimake -f Makefile all
```

### Display the selected Makefile

```sh
./minimake -p
```

## Example

A simple Makefile can look like this:

```make
CC = cc
CFLAGS = -Wall -Wextra

all: hello

hello:
	@echo "Hello from MiniMakefile"
```

Run it with:

```sh
./minimake
```

The command line must start with a tab, as in a regular Makefile.

## Supported variables

MiniMakefile supports simple variable assignments:

```make
NAME = hello
MESSAGE = Hello world

all:
	echo $(MESSAGE)
	echo ${NAME}
```

Single-character variables are also supported:

```make
X = value

all:
	echo $X
```

Variable values can use other variables:

```make
NAME = MiniMakefile
MESSAGE = Hello $(NAME)

all:
	echo $(MESSAGE)
```

## Automatic variables

Commands can use automatic variables:

```make
all: first second

all:
	echo $@
	echo $<
	echo $^
```

The supported automatic variables are:

| Variable | Meaning |
| --- | --- |
| `$@` | Name of the current target |
| `$<` | First dependency |
| `$^` | All dependencies |

## How it works

MiniMakefile follows a simple build process:

1. Read the selected Makefile.
2. Remove comments and empty lines.
3. Read variable assignments.
4. Expand variables.
5. Parse targets, dependencies and commands.
6. Build dependencies first.
7. Compare file modification times.
8. Execute commands using `/bin/sh`.

This keeps the implementation small and easy to read.

## Tests

Run the complete test suite with:

```sh
make check
```

The tests contain 30 scenarios divided into four groups:

- 11 variable tests;
- 7 rule and dependency tests;
- 5 automatic variable tests;
- 7 command execution tests.

The tests check:

- variable expansion;
- recursive variables;
- targets;
- dependencies;
- automatic variables;
- command execution;
- silent commands;
- missing dependencies;
- failed commands;
- timestamp behaviour.

## Formatting

Format the source files with:

```sh
make format
```

Check the formatting without changing files:

```sh
make check-format
```

## Cleaning

Remove object files:

```sh
make clean
```

Remove all generated files:

```sh
make fclean
```

Rebuild the project from scratch:

```sh
make re
```

## Project structure

```text
.
├── .clang-format
├── .github/
│   └── workflows/
│       └── ci.yml
├── .gitignore
├── LICENSE
├── Makefile
├── README.md
├── src/
│   ├── excecuter_commandes.c
│   ├── lire_fichier.c
│   ├── main.c
│   ├── minimake.h
│   ├── nettoyage.c
│   ├── target.c
│   └── variables.c
└── tests/
    ├── Makefile-Excution/
    ├── Makefile-Rules/
    ├── Makefile-Variables/
    ├── Makefile-Variables-Special/
    └── test.sh
```

Each source file has a specific responsibility:

- `main.c`: command-line options and program entry point;
- `lire_fichier.c`: reading Makefile contents;
- `nettoyage.c`: cleaning and splitting file contents;
- `variables.c`: variable storage and expansion;
- `target.c`: target and dependency parsing;
- `excecuter_commandes.c`: dependency building and command execution;
- `minimake.h`: shared structures and function declarations.

## Limitations

MiniMakefile does not implement the complete GNU Make language.

It currently does not support:

- pattern rules;
- parallel builds;
- `.PHONY`;
- included Makefiles;
- wildcard expansion;
- complete command-line variable handling;
- the complete GNU Make syntax;
- advanced dependency cycle diagnostics.

These limitations are intentional. The goal is to understand the main ideas behind a build tool with a small C program.

## Continuous integration

GitHub Actions automatically runs:

```sh
make
make check
make check-format
make fclean
```

The workflow is defined in:

```text
.github/workflows/ci.yml
```

## License

MiniMakefile is distributed under the MIT License.

See the [LICENSE](LICENSE) file for more information.
