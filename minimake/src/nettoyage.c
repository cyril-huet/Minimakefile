#include "minimake.h"

static int taille(char *file)
{
    int index = 0;
    while (file[index] != '\0')
    {
        index++;
    }
    return index;
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

static int ligne_vide(char *ligne)
{
    int index = 0;
    while (ligne[index] != '\0')
    {
        if (ligne[index] != ' ' && ligne[index] != '\t' && ligne[index] != '\n')
        {
            return 0;
        }
        index++;
    }
    return 1;
}

static char *supprimer_commentaire(char *fichier)
{
    int index = taille(fichier);
    if (index <= 0)
    {
        return NULL;
    }
    char *res = malloc(index + 1);
    if (res == NULL)
    {
        return NULL;
    }
    int index2 = 0;
    int index3 = 0;
    while (index2 < index)
    {
        if (fichier[index2] == '#')
        {
            while (index2 < index && fichier[index2] != '\n')
            {
                index2++;
            }
            continue;
        }
        res[index3] = fichier[index2];
        index2++;
        index3++;
    }
    res[index3] = '\0';
    return res;
}

static char *supprimer_ligne_inutile(char *fichier)
{
    int index = taille(fichier);
    if (index <= 0)
    {
        return NULL;
    }
    char *res = malloc(index + 1);
    if (res == NULL)
    {
        return NULL;
    }
    int index2 = 0;
    int index3 = 0;
    while (index2 < index)
    {
        int sauvegarde = index2;
        while (index2 < index && fichier[index2] != '\n')
        {
            index2++;
        }
        if (ligne_vide(fichier + sauvegarde) == 0)
        {
            for (int i = sauvegarde; i < index2; i++)
            {
                res[index3] = fichier[i];
                index3++;
            }
            if (index2 < index && fichier[index2] == '\n')
            {
                res[index3] = '\n';
                index3++;
            }
        }
        if (index2 < index)
        {
            index2++;
        }
    }
    res[index3] = '\0';
    return res;
}

static char *supprimer_espace_double_inutile(char *ligne)
{
    if (ligne[0] == '\t')
    {
        return my_copie(ligne);
    }
    int taille2 = taille(ligne);
    char *res = malloc(taille2 + 1);
    if (res == NULL)
    {
        return NULL;
    }
    int index1 = 0;
    int index2 = 0;
    while (ligne[index1] != '\0')
    {
        res[index2] = ligne[index1];
        index2++;
        if (ligne[index1] == ' ')
        {
            while (ligne[index1 + 1] == ' ')
            {
                index1++;
            }
        }
        index1++;
    }

    res[index2] = '\0';
    return res;
}

static char *supprimer_espace_debut_fin_ligne_inutile(char *ligne)
{
    if (ligne[0] == '\t')
    {
        return my_copie(ligne);
    }
    int index = taille(ligne);
    if (index <= 0)
    {
        return my_copie("");
    }
    char *res = malloc(index + 1);
    if (res == NULL)
    {
        return NULL;
    }
    int index1 = 0;
    int index2 = 0;
    while (index1 < index && ligne[index1] == ' ')
    {
        index1++;
    }
    while (index - 1 >= index1 && ligne[index - 1] == ' ')
    {
        index--;
    }
    int temps = index - 1;
    while (index1 <= temps)
    {
        res[index2] = ligne[index1];
        index2++;
        index1++;
    }
    res[index2] = '\0';
    return res;
}

static char *supprimer_espace_autour_operateurs(char *ligne)
{
    if (ligne[0] == '\t')
    {
        return my_copie(ligne);
    }

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
    int index2 = 0;
    int index3 = 0;
    while (index2 < index)
    {
        if (ligne[index2] == ' ' || ligne[index2] == '\t')
        {
            if ((index2 + 1 < index
                 && (ligne[index2 + 1] == '=' || ligne[index2 + 1] == ':'))
                || (index2 > 0
                    && (ligne[index2 - 1] == '=' || ligne[index2 - 1] == ':')))
            {
                index2++;
                continue;
            }
        }

        res[index3] = ligne[index2];
        index2++;
        index3++;
    }
    res[index3] = '\0';

    return res;
}

static char *nettoyer_ligne(char *ligne)
{
    char *temps = supprimer_espace_double_inutile(ligne);
    char *res = supprimer_espace_debut_fin_ligne_inutile(temps);
    free(temps);
    temps = supprimer_espace_autour_operateurs(res);
    free(res);
    res = temps;
    return res;
}

static char *supprimer_espace_inutile(char *fichier)
{
    int taillef = taille(fichier);
    if (taillef <= 0)
    {
        return NULL;
    }
    char *res = malloc(taillef + 1);
    if (res == NULL)
    {
        return NULL;
    }
    int index_fichier = 0;
    int index_res = 0;
    while (index_fichier < taillef)
    {
        int taille_ligne = 0;
        int temps = index_fichier + taille_ligne;
        while (temps < taillef && fichier[temps] != '\n')
        {
            taille_ligne++;
            temps = index_fichier + taille_ligne;
        }
        char *ligne = malloc(taille_ligne + 1);
        if (ligne == NULL)
        {
            free(res);
            return NULL;
        }
        for (int index = 0; index < taille_ligne; index++)
        {
            ligne[index] = fichier[index_fichier + index];
        }
        ligne[taille_ligne] = '\0';
        char *nettoyee = nettoyer_ligne(ligne);
        free(ligne);
        int index2 = 0;
        while (nettoyee[index2] != '\0')
        {
            res[index_res] = nettoyee[index2];
            index_res++;
            index2++;
        }
        free(nettoyee);
        if (index_fichier + taille_ligne < taillef
            && fichier[index_fichier + taille_ligne] == '\n')
        {
            res[index_res] = '\n';
            index_res++;
        }
        index_fichier += (taille_ligne + 1);
    }
    res[index_res] = '\0';
    return res;
}

char *nettoyage(char *fichier)
{
    char *resultat = supprimer_commentaire(fichier);
    if (resultat == NULL)
    {
        return my_copie(fichier);
    }
    char *res = supprimer_ligne_inutile(resultat);
    free(resultat);
    if (res == NULL)
    {
        return my_copie(fichier);
    }
    resultat = supprimer_espace_inutile(res);
    free(res);
    return resultat;
}
