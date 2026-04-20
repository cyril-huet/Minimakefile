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
static int a_affiche(char *commande)
{
    if (commande == NULL)
    {
        return 0;
    }
    if (commande[0] == '\0')
    {
        return 0;
    }
    int index = 0;
    while (commande[index] == '\t' || commande[index] == ' ')
    {
        index++;
    }
    if (commande[index] == '@')
    {
        return 1;
    }
    return 0;
}

int excecuter_commande(char *commande)
{
    if (commande == NULL)
    {
        return 0;
    }
    if (commande[0] == '\0')
    {
        return 0;
    }
    int affichager = a_affiche(commande);
    int index1 = 0;
    while (commande[index1] == '\t')
    {
        index1++;
    }
    while (commande[index1] == '\t' || commande[index1] == ' ')
    {
        index1++;
    }
    if (commande[index1] == '@')
    {
        index1++;
    }
    char *res = my_copie(commande + index1);
    if (res == NULL)
    {
        return 0;
    }
    if (affichager == 0)
    {
        printf("%s\n", res);
        fflush(stdout);
        fflush(stderr);
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        free(res);
        exit(2);
    }
    if (pid == 0)
    {
        execl("/bin/sh", "sh", "-c", res, NULL);
        exit(127);
    }
    int st = 0;
    waitpid(pid, &st, 0);
    free(res);
    if (!WIFEXITED(st))
    {
        exit(2);
    }
    if (WEXITSTATUS(st) != 0)
    {
        exit(2);
    }
    return 0;
}
static int est_construire(struct construite *list, char *nom)
{
    struct construite *res = list;
    while (res != NULL)
    {
        if (strcmp(res->nom, nom) == 0)
        {
            return 1;
        }
        res = res->next;
    }
    return 0;
}
static struct construite *ajouter_construit(struct construite *liste, char *nom)
{
    struct construite *res = malloc(sizeof(struct construite));
    if (res == NULL)
    {
        return liste;
    }
    res->nom = my_copie(nom);
    res->next = liste;
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
static int dois_etre_construit(struct regles *target,
                               struct regles **regles_du_fichier)
{
    if (target == NULL)
    {
        return 0;
    }
    struct stat temps;
    int test = stat(target->target, &temps);
    if (target->depandance == NULL || target->depandance->nombre == 0)
    {
        return test != 0;
    }
    if (target->commandes->nombre == 0 && test == 0)
    {
        return 0;
    }
    if (test != 0)
    {
        return 1;
    }
    time_t temps_targer = temps.st_mtime;
    int nombre_depancand = target->depandance->nombre;
    for (int i = 0; i < nombre_depancand; i++)
    {
        char *dependance = target->depandance->var[i];
        if (dependance == NULL)
        {
            continue;
        }
        struct stat temps_dependance;
        int test2 = stat(dependance, &temps_dependance);
        if (test2 != 0)
        {
            int index_chercher_regles = 0;
            int flag_regles = 0;
            while (regles_du_fichier[index_chercher_regles] != NULL)
            {
                if (strcmp(regles_du_fichier[index_chercher_regles]->target,
                           dependance)
                    == 0)
                {
                    flag_regles = 1;
                    break;
                }
                index_chercher_regles++;
            }
            if (flag_regles == 0)
            {
                return 1;
            }
            else
            {
                continue;
            }
        }
        if (temps_dependance.st_mtime > temps_targer)
        {
            return 1;
        }
    }
    return 0;
}

static int taille_targer(struct regles *target)
{
    int res = 0;
    if (target->target != NULL)
    {
        res += taille(target->target);
    }
    if (target->depandance != NULL)
    {
        int taiille = target->depandance->nombre;
        for (int i = 0; i < taiille; i++)
        {
            if (target->depandance->var[i] != NULL)
            {
                res += taille(target->depandance->var[i]) + 1;
            }
        }
    }
    res += 1;
    return res;
}
static int auxiliaaire_remplacer_variable(char *res, int index1,
                                          struct regles *target)
{
    int tailledesdepancnade = target->depandance->nombre;
    for (int j = 0; j < tailledesdepancnade; j++)
    {
        char *temps3 = target->depandance->var[j];
        int index5 = taille(temps3);
        for (int i = 0; i < index5; i++)
        {
            res[index1] = temps3[i];
            index1++;
        }
        res[index1] = ' ';
        index1++;
    }
    return index1;
}
static char *remplacer_variable(char *commande, struct regles *target)
{
    if (commande == NULL)
    {
        return my_copie(commande);
    }
    if (target == NULL)
    {
        return my_copie(commande);
    }
    char *res = malloc(taille_targer(target) + taille(commande) + 1);
    if (res == NULL)
    {
        return NULL;
    }
    int index1 = 0;
    int index2 = taille(commande);
    for (int i = 0; i < index2; i++)
    {
        if (commande[i] == '$' && commande[i + 1] != '\0')
        {
            char temps = commande[i + 1];
            if (temps == '@')
            {
                i++;
                int temps2 = taille(target->target);
                for (int j = 0; j < temps2; j++)
                {
                    res[index1] = target->target[j];
                    index1++;
                }
                continue;
            }
            if (temps == '<')
            {
                i++;
                if (target->depandance->nombre != 0)
                {
                    char *temps4 = target->depandance->var[0];
                    int index7 = taille(temps4);
                    for (int j = 0; j < index7; j++)
                    {
                        res[index1] = temps4[j];
                        index1++;
                    }
                }
                continue;
            }
            if (temps == '^')
            {
                i++;
                index1 = auxiliaaire_remplacer_variable(res, index1, target);
                continue;
            }
        }
        else
        {
            res[index1] = commande[i];
            index1++;
        }
    }
    res[index1] = '\0';
    index1++;

    return res;
}
static int auxiliaire_executer_commandes(struct regles *target)
{
    int index4 = 0;
    while (index4 < target->commandes->nombre)
    {
        char *commande_finale =
            remplacer_variable(target->commandes->var[index4], target);
        excecuter_commande(commande_finale);
        index4++;
        free(commande_finale);
    }
    return 0;
}

static void auxiliaire_tout_construire(char *copie,
                                       struct regles **regles_du_fichier,
                                       struct construite **liste)
{
    struct regles *est_target = NULL;
    int index3 = 0;

    while (regles_du_fichier[index3] != NULL && est_target == NULL)
    {
        if (regles_du_fichier[index3]->target != NULL
            && strcmp(regles_du_fichier[index3]->target, copie) == 0)
        {
            est_target = regles_du_fichier[index3];
        }
        index3++;
    }
    if (est_target == NULL)
    {
        struct stat test_temps;
        if (stat(copie, &test_temps) != 0)
        {
            fprintf(stderr,
                    "minimake: *** No rule to make target '%s'. Stop.\n",
                    copie);
            fflush(stderr);
            exit(2);
        }
    }

    if (est_target != NULL)
    {
        tout_construire(est_target, regles_du_fichier, liste);
    }
}

int tout_construire(struct regles *target, struct regles **regles_du_fichier,
                    struct construite **liste)
{
    if (target == NULL)
    {
        return 1;
    }
    if (est_construire(*liste, target->target) == 1)
    {
        printf("minimake: '%s' is up to date.\n", target->target);
        return 1;
    }
    *liste = ajouter_construit(*liste, target->target);
    int index1 = 0;
    int index2 = 0;
    index1 += target->depandance->nombre;
    while (index2 < index1)
    {
        char *copie = target->depandance->var[index2];
        if (copie == NULL)
        {
            index2++;
            continue;
        }
        if (est_construire(*liste, copie))
        {
            index2++;
            continue;
        }

        auxiliaire_tout_construire(copie, regles_du_fichier, liste);
        index2++;
    }
    int flag = dois_etre_construit(target, regles_du_fichier);
    if (flag == 1)
    {
        if (target->commandes->nombre == 0)
        {
            printf("minimake: Nothing to be done for '%s'.\n", target->target);
            return 2;
        }
        return auxiliaire_executer_commandes(target);
    }
    if (target->commandes->nombre == 0)
    {
        printf("minimake: Nothing to be done for '%s'.\n", target->target);
    }
    else
    {
        printf("minimake: '%s' is up to date.\n", target->target);
    }

    return 0;
}
