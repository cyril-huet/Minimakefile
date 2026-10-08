# Compiler
CC = cc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Werror -Wvla

# Program and source files
SRC = src/excecuter_commandes.c src/lire_fichier.c \
      src/main.c src/nettoyage.c \
      src/target.c src/variables.c

OBJ = $(SRC:.c=.o)

TARGET = minimake

# Build the program
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c src/minimake.h
	$(CC) $(CFLAGS) -I src -c $< -o $@

# Run the test suite
check: $(TARGET)
	sh tests/test.sh

# Format the C files
format:
	clang-format -i $(SRC) src/minimake.h

check-format:
	clang-format --dry-run -Werror $(SRC) src/minimake.h

# Remove generated files
clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all check format check-format clean fclean re
