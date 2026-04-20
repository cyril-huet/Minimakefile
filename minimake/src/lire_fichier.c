#include "minimake.h"

char *lire_fichier(char *fichier)
{
    FILE *file = fopen(fichier, "r");
    if (file == NULL)
    {
        return NULL;
    }
    int index = 0;
    int index2 = 0;
    while (index2 != -1)
    {
        index2 = fgetc(file);
        index++;
    }
    char *res = malloc(index);
    if (res == NULL)
    {
        fclose(file);
        return NULL;
    }
    fclose(file);
    file = fopen(fichier, "r");
    if (file == NULL)
    {
        return NULL;
    }
    index -= 1;
    int index3 = 0;
    while (index3 < index)
    {
        res[index3] = fgetc(file);
        index3++;
    }
    res[index] = '\0';
    fclose(file);
    return res;
}
