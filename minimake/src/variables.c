#include "minimake.h"

char **transformer_fichier_en_tableau_de_ligne(char *fichier,
                                               int nombre_de_ligne)
{
    if (fichier == NULL)
    {
        return NULL;
    }
    if (nombre_de_ligne == 0)
    {
        return NULL;
    }
    char **res_ligne = malloc((nombre_de_ligne + 1) * sizeof(char *));
    if (res_ligne == NULL)
    {
        return NULL;
    }
    int index = 0;
    for (int i = 0; i < nombre_de_ligne; i++)
    {
        int debut = index;
        int taille_ligne = 0;
        while (fichier[index] != '\n' && fichier[index] != '\0')
        {
            index++;
            taille_ligne++;
        }
        char *ligne_temporaire = malloc(taille_ligne + 1);
        if (ligne_temporaire == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(res_ligne[j]);
            }
            free(res_ligne);
            return NULL;
        }
        for (int j = 0; j < taille_ligne; j++)
        {
            ligne_temporaire[j] = fichier[debut + j];
        }
        ligne_temporaire[taille_ligne] = '\0';
        res_ligne[i] = ligne_temporaire;
        if (fichier[index] == '\n')
        {
            index++;
        }
    }
    res_ligne[nombre_de_ligne] = NULL;
    return res_ligne;
}

int nombre_de_ligne(char *fichier)
{
    if (fichier == NULL)
    {
        return 0;
    }
    int res = 0;
    int index = 0;
    while (fichier[index] != '\0')
    {
        if (fichier[index] == '\n')
        {
            res++;
        }
        index++;
    }
    if (index > 0)
    {
        if (fichier[index - 1] != '\n')
        {
            res++;
        }
    }
    return res;
}
static int taille(char *file)
{
    int index = 0;
    while (file[index] != '\0')
    {
        index++;
    }
    return index;
}
static char *nouveau_fichier(char **ligne, int nombre_de_ligne)
{
    if (ligne == NULL || nombre_de_ligne == 0)
    {
        return NULL;
    }
    int taille_res = 0;
    for (int i = 0; i < nombre_de_ligne; i++)
    {
        if (ligne[i])
        {
            taille_res += taille(ligne[i]) + 1;
        }
    }
    char *res = malloc(taille_res + 1);
    if (res == NULL)
    {
        return NULL;
    }
    int index = 0;
    for (int i = 0; i < nombre_de_ligne; i++)
    {
        if (ligne[i] == NULL)
        {
            continue;
        }
        int taille2 = taille(ligne[i]);
        for (int j = 0; j < taille2; j++)
        {
            res[index] = ligne[i][j];
            index++;
        }
        res[index] = '\n';
        index++;
        free(ligne[i]);
    }
    free(ligne);
    res[index] = '\0';
    return res;
}

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

int nb_de_variable_dans_le_fichier(char *fichier)
{
    if (fichier == NULL)
    {
        return 0;
    }
    int index = 0;
    int res = 0;
    while (fichier[index] != '\0')
    {
        if (fichier[index] == '=')
        {
            res++;
        }
        index++;
    }
    return res;
}

static int valide_variable(char *ligne)
{
    if (ligne[0] == '\0')
    {
        return 0;
    }
    int index = 0;
    while (ligne[index] != '\0' && ligne[index] != '=')
    {
        char caractere = ligne[index];
        if (caractere == ':')
        {
            return 0;
        }
        if (caractere == '#')
        {
            return 0;
        }
        if (caractere == '=')
        {
            return 0;
        }
        if (caractere == ' ')
        {
            return 0;
        }
        index++;
    }
    if (ligne[index] != '=')
    {
        return 0;
    }
    if (index == 0)
    {
        return 0;
    }
    return 1;
}

static struct dictionnaire *create(int nombre_variable)
{
    struct dictionnaire *res =
        malloc(nombre_variable * sizeof(struct dictionnaire));
    if (res == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < nombre_variable; i++)
    {
        res[i].nom = NULL;
        res[i].valeur = NULL;
    }
    return res;
}

static void ajouter(struct dictionnaire *dictionnaire, int nombre_variable,
                    char *nom, char *valeur)
{
    for (int i = 0; i < nombre_variable; i++)
    {
        if (dictionnaire[i].nom == NULL)
        {
            dictionnaire[i].nom = my_copie(nom);
            dictionnaire[i].valeur = my_copie(valeur);
            return;
        }
        else if (strcmp(dictionnaire[i].nom, nom) == 0)
        {
            free(dictionnaire[i].valeur);
            dictionnaire[i].valeur = my_copie(valeur);
            return;
        }
    }
}
static void copie_valeur_dictionnaire(
    struct st_auxiliaire_tableau_regles *st_auxiliaire_tableau_regle, char *res,
    char *valeur_dictionnaire)

{
    int index_res = 0;
    while (valeur_dictionnaire[index_res] != '\0')
    {
        res[*st_auxiliaire_tableau_regle->index] =
            valeur_dictionnaire[index_res];
        (*st_auxiliaire_tableau_regle->index)++;
        index_res++;
    }
}
static int auxiliaire_remplacer_variable(
    char *ligne, char *res,
    struct st_auxiliaire_tableau_regles *st_auxiliaire_tableau_regle, char fin)
{
    int index_debut = *st_auxiliaire_tableau_regle->index_ligne + 1;
    int index_fin = index_debut;
    while (ligne[index_fin] != '\0' && ligne[index_fin] != fin)
    {
        index_fin++;
    }
    int taille_de_la_variable = index_fin - index_debut;
    char *variable = malloc(taille_de_la_variable + 1);
    if (variable == NULL)
    {
        return -1;
    }
    for (int i = 0; i < taille_de_la_variable; i++)
    {
        variable[i] = ligne[index_debut + i];
    }
    variable[taille_de_la_variable] = '\0';
    char *valeur_dictionnaire = NULL;
    for (int j = 0; j < st_auxiliaire_tableau_regle->nombre_variable; j++)
    {
        if (st_auxiliaire_tableau_regle->dictionnaire1[j].nom != NULL
            && strcmp(st_auxiliaire_tableau_regle->dictionnaire1[j].nom,
                      variable)
                == 0)
        {
            valeur_dictionnaire =
                st_auxiliaire_tableau_regle->dictionnaire1[j].valeur;
            break;
        }
    }
    if (valeur_dictionnaire == NULL)
    {
        valeur_dictionnaire = getenv(variable);
    }
    if (valeur_dictionnaire != NULL)
    {
        copie_valeur_dictionnaire(st_auxiliaire_tableau_regle, res,
                                  valeur_dictionnaire);
    }
    free(variable);
    if (ligne[index_fin] == fin)
    {
        index_fin++;
    }
    *st_auxiliaire_tableau_regle->index_ligne = index_fin;
    return 0;
}
static void copie_variable_dollar(
    struct st_auxiliaire_tableau_regles *st_auxiliaire_tableau_regle, char *res,
    char *valeur_dictionnaire)
{
    int index_res = 0;
    while (valeur_dictionnaire[index_res] != '\0')
    {
        res[*st_auxiliaire_tableau_regle->index] =
            valeur_dictionnaire[index_res];
        (*st_auxiliaire_tableau_regle->index)++;
        index_res++;
    }
    (*st_auxiliaire_tableau_regle->index_ligne)++;
}

static int auxiliaire_copier_variable(
    char type, char *res,
    struct st_auxiliaire_tableau_regles *st_auxiliaire_tableau_regle)
{
    char res_variable[2];
    res_variable[0] = type;
    res_variable[1] = '\0';
    char *valeur_dictionnaire = NULL;
    for (int j = 0; j < st_auxiliaire_tableau_regle->nombre_variable; j++)
    {
        if (st_auxiliaire_tableau_regle->dictionnaire1[j].nom != NULL
            && strcmp(st_auxiliaire_tableau_regle->dictionnaire1[j].nom,
                      res_variable)
                == 0)
        {
            valeur_dictionnaire =
                st_auxiliaire_tableau_regle->dictionnaire1[j].valeur;
            break;
        }
    }
    if (valeur_dictionnaire == NULL)
    {
        (*st_auxiliaire_tableau_regle->index_ligne)++;
        return 0;
    }
    copie_variable_dollar(st_auxiliaire_tableau_regle, res,
                          valeur_dictionnaire);
    return 0;
}

static int auxiliaire_remplacer_apresdollar(
    char *ligne, char *res,
    struct st_auxiliaire_tableau_regles *st_auxiliaire_tableau_regle)
{
    (*st_auxiliaire_tableau_regle->index_ligne)++;
    char type = ligne[*st_auxiliaire_tableau_regle->index_ligne];
    if ((type >= 'A' && type <= 'Z') || (type >= 'a' && type <= 'z')
        || (type >= '0' && type <= '9'))
    {
        return auxiliaire_copier_variable(type, res,
                                          st_auxiliaire_tableau_regle);
    }
    if (type == '(')
    {
        int resultat = auxiliaire_remplacer_variable(
            ligne, res, st_auxiliaire_tableau_regle, ')');
        if (resultat == -1)
        {
            return -1;
        }
        return 0;
    }
    if (type == '{')
    {
        int resultat = auxiliaire_remplacer_variable(
            ligne, res, st_auxiliaire_tableau_regle, '}');
        if (resultat == -1)
        {
            return -1;
        }
        return 0;
    }
    res[*st_auxiliaire_tableau_regle->index] = '$';
    (*st_auxiliaire_tableau_regle->index)++;
    if (type != '\0')
    {
        res[*st_auxiliaire_tableau_regle->index] = type;
        (*st_auxiliaire_tableau_regle->index_ligne)++;
        (*st_auxiliaire_tableau_regle->index)++;
    }
    return 0;
}

static void auxiliaire_boucle_while(
    char *ligne, char *res,
    struct st_auxiliaire_tableau_regles *st_auxiliaire_tableau_regle)
{
    while (ligne[*st_auxiliaire_tableau_regle->index_ligne] != '\0')
    {
        if (ligne[*st_auxiliaire_tableau_regle->index_ligne] == '$'
            && ligne[*st_auxiliaire_tableau_regle->index_ligne + 1] == '$')
        {
            res[*st_auxiliaire_tableau_regle->index] = '$';
            (*st_auxiliaire_tableau_regle->index)++;
            *st_auxiliaire_tableau_regle->index_ligne += 2;
            continue;
        }

        if (ligne[*st_auxiliaire_tableau_regle->index_ligne] == '$')
        {
            int resultat = auxiliaire_remplacer_apresdollar(
                ligne, res, st_auxiliaire_tableau_regle);

            if (resultat == -1)
            {
                return;
            }
            continue;
        }

        res[*st_auxiliaire_tableau_regle->index] =
            ligne[*st_auxiliaire_tableau_regle->index_ligne];
        (*st_auxiliaire_tableau_regle->index)++;
        (*st_auxiliaire_tableau_regle->index_ligne)++;
    }
}

static char *remplacer_variables_dans_ligne_complet(
    char *ligne, struct dictionnaire *dictionnaire1, int nombre_variable)
{
    if (ligne == NULL)
    {
        return NULL;
    }
    int taille_ligne = taille(ligne);
    for (int i = 0; i < nombre_variable; i++)
    {
        if (dictionnaire1[i].nom != NULL)
        {
            taille_ligne += taille(dictionnaire1[i].valeur);
        }
    }
    taille_ligne += 1;
    char *res = malloc(taille_ligne);
    if (res == NULL)
    {
        return NULL;
    }
    int index = 0;
    int index_ligne = 0;
    if (ligne[index_ligne] == '\t')
    {
        res[index] = '\t';
        index++;
        index_ligne++;
    }
    struct st_auxiliaire_tableau_regles st_auxiliaire_tableau_regle = {
        &index, &index_ligne, dictionnaire1, nombre_variable
    };
    if (ligne == NULL)
    {
        free(res);
        return NULL;
    }
    auxiliaire_boucle_while(ligne, res, &st_auxiliaire_tableau_regle);

    res[index] = '\0';
    return res;
}

static char *auxiliaire_remplir_dicto_variable_nul(char *fichier)
{
    int nombre_ligne = nombre_de_ligne(fichier);
    char **lignes =
        transformer_fichier_en_tableau_de_ligne(fichier, nombre_ligne);
    if (lignes == NULL)
    {
        return my_copie(fichier);
    }
    for (int i = 0; i < nombre_ligne; i++)
    {
        char *temps =
            remplacer_variables_dans_ligne_complet(lignes[i], NULL, 0);
        free(lignes[i]);
        lignes[i] = temps;
    }
    char *res = nouveau_fichier(lignes, nombre_ligne);
    return res;
}

static void auxiliaire_remplir_dicto_ajouter(struct dictionnaire *res,
                                             int nombre_variable, char **lignes,
                                             int nombre_de_ligne2)
{
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < nombre_de_ligne2; i++)
        {
            if (lignes[i] && strchr(lignes[i], '=') != NULL)
            {
                if (valide_variable(lignes[i]))
                {
                    char *valeur2 = strchr(lignes[i], '=');
                    int taille_du_nom = valeur2 - lignes[i];

                    char *nom = malloc(taille_du_nom + 1);
                    strncpy(nom, lignes[i], taille_du_nom);
                    nom[taille_du_nom] = '\0';
                    char *valeur = my_copie(valeur2 + 1);
                    char *nom_ligne = remplacer_variables_dans_ligne_complet(
                        nom, res, nombre_variable);
                    char *valeur_ligne = remplacer_variables_dans_ligne_complet(
                        valeur, res, nombre_variable);

                    ajouter(res, nombre_variable, nom_ligne, valeur_ligne);
                    free(nom_ligne);
                    free(valeur_ligne);
                    free(nom);
                    free(valeur);
                }
            }
        }
    }
}
char *remplir_dictionnaire_variable(char *fichier, int nombre_variable)
{
    if (nombre_variable == 0)
    {
        return auxiliaire_remplir_dicto_variable_nul(fichier);
    }
    struct dictionnaire *res = create(nombre_variable);
    if (res == NULL)
    {
        return NULL;
    }
    int nombre_de_ligne2 = nombre_de_ligne(fichier);
    char **lignes =
        transformer_fichier_en_tableau_de_ligne(fichier, nombre_de_ligne2);
    if (lignes == NULL)
    {
        return NULL;
    }

    auxiliaire_remplir_dicto_ajouter(res, nombre_variable, lignes,
                                     nombre_de_ligne2);
    for (int i = 0; i < nombre_de_ligne2; i++)
    {
        if (lignes[i] == NULL)
        {
            continue;
        }
        char *nouveau = remplacer_variables_dans_ligne_complet(lignes[i], res,
                                                               nombre_variable);
        free(lignes[i]);
        lignes[i] = nouveau;
    }
    char *resulat_fichier = nouveau_fichier(lignes, nombre_de_ligne2);
    for (int i = 0; i < nombre_variable; i++)
    {
        free(res[i].nom);
        free(res[i].valeur);
    }
    free(res);
    return resulat_fichier;
}
