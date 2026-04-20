#include "minimake.h"

static char *my_copie(char *ligne)
{
    int index = 0;
    while (ligne[index] != '\0')
    {
        index++;
    }
    char *res = malloc(index + 1);
    if (res == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < index; i++)
    {
        res[i] = ligne[i];
    }
    res[index] = '\0';
    return res;
}

static struct liste *creer_liste(void)
{
    struct liste *res = malloc(sizeof(struct liste));
    res->var = NULL;
    res->nombre = 0;
    return res;
}

static struct regles *creer_rule(char *target)
{
    if (target == NULL)
    {
        return NULL;
    }
    struct regles *res = malloc(sizeof(struct regles));
    if (res == NULL)
    {
        return NULL;
    }
    res->target = my_copie(target);
    res->depandance = creer_liste();
    res->commandes = creer_liste();
    return res;
}

static void ajouter_list(struct liste *list, char *var)
{
    if (list == NULL)
    {
        return;
    }
    if (var == NULL)
    {
        return;
    }
    char **res = realloc(list->var, sizeof(char *) * (list->nombre + 1));
    if (res == NULL)
    {
        return;
    }

    list->var = res;
    list->var[list->nombre] = my_copie(var);
    list->nombre++;
}
static int nombre_de_target(char **lignes, int nombre_de_ligne)
{
    if (lignes == NULL)
    {
        return 0;
    }
    if (nombre_de_ligne == 0)
    {
        return 0;
    }
    int res = 0;
    int index = 0;

    while (index < nombre_de_ligne)
    {
        if (lignes[index] && strchr(lignes[index], ':') != NULL)
        {
            res++;
        }
        index++;
    }
    return res;
}

static void auxiliaire_ajouter_des_depandance(struct regles *regles2,
                                              char *ligne,
                                              int dependancesnombre)
{
    while (ligne[dependancesnombre] != '\0')
    {
        int compteur = 0;
        while (ligne[dependancesnombre + compteur] != ' '
               && ligne[dependancesnombre + compteur] != '\0')
        {
            compteur++;
        }
        if (compteur > 0)
        {
            char *depandancenom = malloc(compteur + 1);
            for (int k = 0; k < compteur; k++)
            {
                depandancenom[k] = ligne[dependancesnombre + k];
            }
            depandancenom[compteur] = '\0';
            ajouter_list(regles2->depandance, depandancenom);
            free(depandancenom);
        }
        dependancesnombre = dependancesnombre + compteur;
        while (ligne[dependancesnombre] == ' '
               && ligne[dependancesnombre] != '\0')
        {
            dependancesnombre++;
        }
    }
}
static void auxiliaire_tableau_regles(char **lignes, int nombre_de_ligne,
                                      struct regles **res, int nombretarget)
{
    int index = 0;
    int indexres = 0;
    while (index < nombre_de_ligne)
    {
        int tailleavantdeuxoint = 0;
        if (lignes[index] == NULL || strchr(lignes[index], ':') == NULL)
        {
            index++;
            continue;
        }
        while (lignes[index][tailleavantdeuxoint] != ':'
               && lignes[index][tailleavantdeuxoint] != '\0')
        {
            tailleavantdeuxoint++;
        }
        char *nom = malloc(tailleavantdeuxoint + 1);
        if (nom == NULL)
        {
            return;
        }
        for (int i = 0; i < tailleavantdeuxoint; i++)
        {
            nom[i] = lignes[index][i];
        }
        nom[tailleavantdeuxoint] = '\0';
        struct regles *regles2 = creer_rule(nom);
        free(nom);
        if (regles2 == NULL)
        {
            return;
        }
        int dependancesnombre = tailleavantdeuxoint + 1;
        while (lignes[index][dependancesnombre] == ' '
               && lignes[index][dependancesnombre] != '\0')
        {
            dependancesnombre++;
        }
        auxiliaire_ajouter_des_depandance(regles2, lignes[index],
                                          dependancesnombre);
        int indexdescommande = index + 1;
        while (indexdescommande < nombre_de_ligne && lignes[indexdescommande]
               && lignes[indexdescommande][0] == '\t')
        {
            ajouter_list(regles2->commandes, lignes[indexdescommande]);
            indexdescommande++;
        }
        res[indexres] = regles2;
        indexres++;
        index = indexdescommande;
    }
    res[nombretarget] = NULL;
}
struct regles **tableau_regles(char **lignes, int nombre_de_ligne)
{
    int nombretarget = nombre_de_target(lignes, nombre_de_ligne);
    if (nombretarget == 0)
    {
        return NULL;
    }
    struct regles **res = malloc(sizeof(struct regles *) * (nombretarget + 1));
    if (res == NULL)
    {
        return NULL;
    }
    auxiliaire_tableau_regles(lignes, nombre_de_ligne, res, nombretarget);
    return res;
}

void liberer_regles(struct regles **regles, int nb_regles)
{
    if (regles == NULL)
    {
        return;
    }
    for (int i = 0; i < nb_regles; i++)
    {
        if (regles[i] == NULL)
        {
            continue;
        }
        free(regles[i]->target);
        if (regles[i]->depandance != NULL)
        {
            int nombre_de_depancance = regles[i]->depandance->nombre;
            for (int j = 0; j < nombre_de_depancance; j++)
            {
                free(regles[i]->depandance->var[j]);
            }
            free(regles[i]->depandance->var);
            free(regles[i]->depandance);
        }
        if (regles[i]->commandes != NULL)
        {
            int nombre_de_depancance = regles[i]->commandes->nombre;
            for (int j = 0; j < nombre_de_depancance; j++)
            {
                free(regles[i]->commandes->var[j]);
            }
            free(regles[i]->commandes->var);
            free(regles[i]->commandes);
        }
        free(regles[i]);
    }
    free(regles);
}
