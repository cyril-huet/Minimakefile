CC = gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
TARGET = minimake
all: minimake


minimake: src/excecuter_commandes.o src/lire_fichier.o src/main.o src/nettoyage.o src/target.o src/variables.o
		$(CC)  -o minimake src/excecuter_commandes.o src/lire_fichier.o src/main.o src/nettoyage.o src/target.o src/variables.o

check:
		 ./tests/test.sh
clean:
		$(RM) $(OBJS) $(TARGET) src/excecuter_commandes.o src/lire_fichier.o src/main.o src/nettoyage.o src/target.o src/variables.o

