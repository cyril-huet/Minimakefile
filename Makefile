# Compiler
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Werror -Wvla

# Source files
SRC = src/excecuter_commandes.c src/lire_fichier.c \
      src/main.c src/nettoyage.c \
      src/target.c src/variables.c

# Object files
OBJ = $(SRC:.c=.o)

# Program name
TARGET = minimake

# Build the program
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# Compile one source file
%.o: %.c
	$(CC) $(CFLAGS) -I src -c $< -o $@

# Run the tests
check: $(TARGET)
	sh tests/test.sh

# Format the source files
format:
	clang-format -i $(SRC) src/minimake.h

# Check the formatting
check-format:
	clang-format --dry-run -Werror $(SRC) src/minimake.h

# Remove generated files
clean:
	rm -f $(OBJ)
	rm -f $(TARGET)

fclean: clean

re: fclean all

.PHONY: all check format check-format clean fclean re