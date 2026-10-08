#include "minimake.h"

static int string_length(const char *string)
{
    int length = 0;
    while (string[length] != '\0')
    {
        length++;
    }
    return length;
}

static int only_spaces(const char *line)
{
    int i = 0;
    while (line[i] != '\0')
    {
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r')
        {
            return 0;
        }
        i++;
    }
    return 1;
}

static int append_character(char *result, int index, char character)
{
    result[index] = character;
    return index + 1;
}

static int append_normal_line(char *result, int result_index, char *line)
{
    int start = 0;
    int end = string_length(line);

    while (line[start] == ' ' || line[start] == '\t')
    {
        start++;
    }
    while (end > start && (line[end - 1] == ' ' || line[end - 1] == '\t' ||
                           line[end - 1] == '\r'))
    {
        end--;
    }

    for (int i = start; i < end; i++)
    {
        result[result_index] = line[i];
        result_index++;
    }
    return append_character(result, result_index, '\n');
}

static int append_recipe_line(char *result, int result_index, char *line)
{
    int length = string_length(line);
    for (int i = 0; i < length; i++)
    {
        result[result_index] = line[i];
        result_index++;
    }
    return append_character(result, result_index, '\n');
}

char *nettoyage(char *fichier)
{
    if (fichier == NULL)
    {
        return NULL;
    }

    int length = string_length(fichier);
    char *result = malloc((size_t)length + 2);
    if (result == NULL)
    {
        return NULL;
    }

    int result_index = 0;
    int line_start = 0;
    for (int i = 0; i <= length; i++)
    {
        if (fichier[i] != '\n' && fichier[i] != '\0')
        {
            continue;
        }

        int line_length = i - line_start;
        char *line = malloc((size_t)line_length + 1);
        if (line == NULL)
        {
            free(result);
            return NULL;
        }
        for (int j = 0; j < line_length; j++)
        {
            line[j] = fichier[line_start + j];
        }
        line[line_length] = '\0';

        if (line[0] == '\t')
        {
            result_index = append_recipe_line(result, result_index, line);
        }
        else if (only_spaces(line) == 0)
        {
            char *comment = strchr(line, '#');
            if (comment != NULL)
            {
                *comment = '\0';
            }
            if (only_spaces(line) == 0)
            {
                result_index = append_normal_line(result, result_index, line);
            }
        }

        free(line);
        line_start = i + 1;
    }

    result[result_index] = '\0';
    return result;
}

int nombre_de_ligne(char *fichier)
{
    if (fichier == NULL || fichier[0] == '\0')
    {
        return 0;
    }

    int lines = 0;
    for (int i = 0; fichier[i] != '\0'; i++)
    {
        if (fichier[i] == '\n')
        {
            lines++;
        }
    }
    if (fichier[string_length(fichier) - 1] != '\n')
    {
        lines++;
    }
    return lines;
}

char **transformer_fichier_en_tableau_de_ligne(char *fichier,
                                               int nombre_de_ligne2)
{
    if (fichier == NULL || nombre_de_ligne2 <= 0)
    {
        return NULL;
    }

    char **lines = calloc((size_t)nombre_de_ligne2 + 1, sizeof(char *));
    if (lines == NULL)
    {
        return NULL;
    }

    int line_start = 0;
    int line_number = 0;
    int length = string_length(fichier);
    for (int i = 0; i <= length && line_number < nombre_de_ligne2; i++)
    {
        if (fichier[i] != '\n' && fichier[i] != '\0')
        {
            continue;
        }

        int line_length = i - line_start;
        lines[line_number] = malloc((size_t)line_length + 1);
        if (lines[line_number] == NULL)
        {
            for (int j = 0; j < line_number; j++)
            {
                free(lines[j]);
            }
            free(lines);
            return NULL;
        }
        for (int j = 0; j < line_length; j++)
        {
            lines[line_number][j] = fichier[line_start + j];
        }
        lines[line_number][line_length] = '\0';
        line_number++;
        line_start = i + 1;
    }
    return lines;
}
