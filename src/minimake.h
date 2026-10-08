#ifndef MINIMAKE_H
#define MINIMAKE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* A small dynamic list of strings. */
struct liste
{
    char **var;
    int nombre;
};

/* One variable from the Makefile. */
struct dictionnaire
{
    char *nom;
    char *valeur;
};

/* One target and its rule. */
struct regles
{
    char *target;
    struct liste *depandance;
    struct liste *commandes;
};

/* Targets already visited during one build. */
struct construite
{
    char *nom;
    struct construite *next;
};

struct option_auxiliaire_argument
{
    int tiret_h;
    int tiret_p;
    char *fichier;
};

struct st_auxiliaire_tableau_regles
{
    int *index;
    int *index_ligne;
    struct dictionnaire *dictionnaire1;
    int nombre_variable;
};

char *lire_fichier(char *fichier);
char *nettoyage(char *fichier);

int nombre_de_ligne(char *fichier);
char **transformer_fichier_en_tableau_de_ligne(char *fichier,
                                               int nombre_de_ligne);

int nb_de_variable_dans_le_fichier(char *fichier);
char *remplir_dictionnaire_variable(char *fichier, int nombre_variable);

struct regles **tableau_regles(char **lignes, int nombre_de_ligne);
void liberer_regles(struct regles **regles, int nb_regles);

int excecuter_commande(char *commande);
int tout_construire(struct regles *target, struct regles **regles_du_fichier,
                    struct construite **liste);

#endif /* ! MINIMAKE_H */
