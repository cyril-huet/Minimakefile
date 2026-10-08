#include "minimake.h"

char *lire_fichier(char *fichier)
{
    FILE *file = fopen(fichier, "r");
    if (file == NULL)
    {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0)
    {
        fclose(file);
        return NULL;
    }

    long file_size = ftell(file);
    if (file_size < 0 || fseek(file, 0, SEEK_SET) != 0)
    {
        fclose(file);
        return NULL;
    }

    char *content = malloc((size_t)file_size + 1);
    if (content == NULL)
    {
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(content, 1, (size_t)file_size, file);
    fclose(file);
    if (read_size != (size_t)file_size)
    {
        free(content);
        return NULL;
    }

    content[read_size] = '\0';
    return content;
}
