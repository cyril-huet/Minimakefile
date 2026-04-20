#include "minimake.h"

static void affichage_tiret_p(char *fichier)
{
    printf("%s\n", fichier);
}
static void liberer_construit(struct construite *liste)
{
    while (liste != NULL)
    {
        struct construite *temps = liste->next;
        free(liste->nom);
        free(liste);
        liste = temps;
    }
}

static int auxiliaire_argument(
    int argc, char *argv[],
    struct option_auxiliaire_argument *option_auxiliaire_arguments)
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            option_auxiliaire_arguments->tiret_p = 1;
            continue;
        }
        if (strcmp(argv[i], "-h") == 0)
        {
            option_auxiliaire_arguments->tiret_h = 1;
            continue;
        }
        if (strcmp(argv[i], "-f") == 0)
        {
            i++;
            if (i >= argc)
            {
                fprintf(stderr,
                        "minimake: *** missing filename after '-f'. Stop.\n");
                return 2;
            }
            option_auxiliaire_arguments->fichier = argv[i];
            continue;
        }
        if (strcmp(argv[i], "") == 0)
        {
            fprintf(stderr,
                    "minimake: *** empty string invalid as argument.  Stop.\n");
            return 2;
        }
    }
    return 0;
}
static struct regles **auxiliaire_commande(char *fichier)

{
    char *tableau_fichier = lire_fichier(fichier);
    if (tableau_fichier == NULL)
    {
        printf(
            "make: *** No targets specified and no makefile found.  Stop.\n");
        return NULL;
    }
    char *fichier_nettoye = nettoyage(tableau_fichier);
    int nombre_de_variable = nb_de_variable_dans_le_fichier(fichier_nettoye);
    char *changement_des_variable =
        remplir_dictionnaire_variable(fichier_nettoye, nombre_de_variable);
    int nombre_ligne = nombre_de_ligne(changement_des_variable);
    char **fichier_avec_ligne = transformer_fichier_en_tableau_de_ligne(
        changement_des_variable, nombre_ligne);
    struct regles **regles = tableau_regles(fichier_avec_ligne, nombre_ligne);
    for (int i = 0; i < nombre_ligne; i++)
    {
        free(fichier_avec_ligne[i]);
    }
    free(fichier_avec_ligne);
    free(tableau_fichier);
    free(changement_des_variable);
    free(fichier_nettoye);
    return regles;
}
static void auxiliaire_cible(int argc, char *argv[], struct regles **regles,
                             struct construite **liste)

{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            i++;
            continue;
        }
        if (strcmp(argv[i], "-p") == 0)
        {
            continue;
        }
        if (strcmp(argv[i], "-h") == 0)
        {
            continue;
        }
        int est_trouve = 0;
        int j = 0;
        while (regles[j])
        {
            if (strcmp(argv[i], regles[j]->target) == 0)
            {
                tout_construire(regles[j], regles, liste);
                est_trouve = 1;
                break;
            }
            j++;
        }
        if (est_trouve == 0)
        {
            printf("minimake: *** No rule to make target '%s'. Stop.\n",
                   argv[i]);
        }
    }
}
static int auxiliaire_fichier_null(char **fichier)
{
    char *test_existance_fichier = lire_fichier("makefile");
    if (test_existance_fichier != NULL)
    {
        free(test_existance_fichier);
        *fichier = "makefile";
    }
    else
    {
        *fichier = "Makefile";
        test_existance_fichier = lire_fichier(*fichier);
        if (test_existance_fichier == NULL)
        {
            fprintf(stderr,
                    "minimake: *** No targets specified and no makefile "
                    "found. Stop.\n");
            return 2;
        }
        free(test_existance_fichier);
    }
    return 0;
}
static int auxiliare_flag(int argc, char *argv[])
{
    int flag = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            i++;
            continue;
        }
        if (strcmp(argv[i], "-p") == 0)
        {
            continue;
        }
        if (strcmp(argv[i], "-h") == 0)
        {
            continue;
        }
        else
        {
            flag = 1;
        }
    }
    return flag;
}
int main(int argc, char *argv[])
{
    struct option_auxiliaire_argument tiret = { 0, 0, NULL };
    struct construite *liste = NULL;
    if (auxiliaire_argument(argc, argv, &tiret) == 2)
    {
        return 2;
    }
    if (tiret.tiret_h == 1)
    {
        printf("Help");
        return 0;
    }
    if (tiret.fichier == NULL)
    {
        if (auxiliaire_fichier_null(&tiret.fichier) == 2)
        {
            return 2;
        }
    }
    if (tiret.tiret_p == 1)
    {
        affichage_tiret_p(tiret.fichier);
        return 0;
    }
    struct regles **regles = auxiliaire_commande(tiret.fichier);
    if (regles == NULL)
    {
        printf("pas de target");
        return 2;
    }
    if (regles[0] == NULL)
    {
        printf("pas de target");
        return 2;
    }
    int flag = auxiliare_flag(argc, argv);
    if (flag == 1)
    {
        auxiliaire_cible(argc, argv, regles, &liste);
    }
    else
    {
        tout_construire(regles[0], regles, &liste);
    }
    int nombre = 0;
    while (regles[nombre] != NULL)
    {
        nombre++;
    }
    liberer_construit(liste);
    liberer_regles(regles, nombre);
    return 0;
}
