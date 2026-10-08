#include "minimake.h"

struct output_buffer
{
    char *data;
    int length;
    int capacity;
};

static int text_length(const char *text)
{
    int length = 0;
    while (text[length] != '\0')
    {
        length++;
    }
    return length;
}

static char *copy_text(const char *text)
{
    int length = text_length(text);
    char *copy = malloc((size_t)length + 1);
    if (copy == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < length; i++)
    {
        copy[i] = text[i];
    }
    copy[length] = '\0';
    return copy;
}

static int is_name_character(char character)
{
    if ((character >= 'a' && character <= 'z') ||
        (character >= 'A' && character <= 'Z') ||
        (character >= '0' && character <= '9') || character == '_')
    {
        return 1;
    }
    return 0;
}

static int buffer_grow(struct output_buffer *buffer, int extra)
{
    while (buffer->length + extra + 1 >= buffer->capacity)
    {
        buffer->capacity *= 2;
    }

    char *new_data = realloc(buffer->data, (size_t)buffer->capacity);
    if (new_data == NULL)
    {
        return -1;
    }
    buffer->data = new_data;
    return 0;
}

static int buffer_add_character(struct output_buffer *buffer, char character)
{
    if (buffer_grow(buffer, 1) < 0)
    {
        return -1;
    }
    buffer->data[buffer->length] = character;
    buffer->length++;
    buffer->data[buffer->length] = '\0';
    return 0;
}

static int buffer_add_text(struct output_buffer *buffer, const char *text)
{
    int length = text_length(text);
    if (buffer_grow(buffer, length) < 0)
    {
        return -1;
    }

    for (int i = 0; i < length; i++)
    {
        buffer->data[buffer->length] = text[i];
        buffer->length++;
    }
    buffer->data[buffer->length] = '\0';
    return 0;
}

static char *trim_copy(const char *text, int length)
{
    int start = 0;
    while (start < length && (text[start] == ' ' || text[start] == '\t'))
    {
        start++;
    }

    while (length > start &&
           (text[length - 1] == ' ' || text[length - 1] == '\t'))
    {
        length--;
    }

    char *copy = malloc((size_t)(length - start) + 1);
    if (copy == NULL)
    {
        return NULL;
    }
    for (int i = start; i < length; i++)
    {
        copy[i - start] = text[i];
    }
    copy[length - start] = '\0';
    return copy;
}

static int assignment_position(const char *line)
{
    if (line[0] == '\t')
    {
        return -1;
    }

    for (int i = 0; line[i] != '\0'; i++)
    {
        if (line[i] == ':')
        {
            return -1;
        }
        if (line[i] == '=')
        {
            return i;
        }
    }
    return -1;
}

static struct dictionnaire *find_variable(struct dictionnaire *variables,
                                          int count, const char *name)
{
    for (int i = 0; i < count; i++)
    {
        if (variables[i].nom != NULL && strcmp(variables[i].nom, name) == 0)
        {
            return &variables[i];
        }
    }
    return NULL;
}

static int set_variable(struct dictionnaire *variables, int count,
                        const char *name, const char *value)
{
    struct dictionnaire *variable = find_variable(variables, count, name);
    if (variable == NULL)
    {
        for (int i = 0; i < count; i++)
        {
            if (variables[i].nom == NULL)
            {
                variable = &variables[i];
                break;
            }
        }
    }
    if (variable == NULL)
    {
        return -1;
    }

    char *new_name = copy_text(name);
    char *new_value = copy_text(value);
    if (new_name == NULL || new_value == NULL)
    {
        free(new_name);
        free(new_value);
        return -1;
    }

    free(variable->nom);
    free(variable->valeur);
    variable->nom = new_name;
    variable->valeur = new_value;
    return 0;
}

static char *expand_text(const char *text, struct dictionnaire *variables,
                         int count, int depth);

static char *variable_value(const char *name, struct dictionnaire *variables,
                            int count, int depth)
{
    struct dictionnaire *variable = find_variable(variables, count, name);
    if (variable == NULL || variable->valeur == NULL)
    {
        return copy_text("");
    }
    if (depth > 20)
    {
        return copy_text(variable->valeur);
    }
    return expand_text(variable->valeur, variables, count, depth + 1);
}

static char *read_variable_name(const char *text, int *index)
{
    int start = *index;
    int end = start;
    char closing = '\0';

    if (text[start] == '(')
    {
        closing = ')';
    }
    else if (text[start] == '{')
    {
        closing = '}';
    }

    if (closing != '\0')
    {
        start++;
        end = start;
        while (text[end] != '\0' && text[end] != closing)
        {
            end++;
        }
        if (text[end] == closing)
        {
            *index = end + 1;
            return trim_copy(text + start, end - start);
        }
        return NULL;
    }

    if (is_name_character(text[start]) == 0)
    {
        return NULL;
    }
    *index = start + 1;
    return trim_copy(text + start, 1);
}

static char *expand_text(const char *text, struct dictionnaire *variables,
                         int count, int depth)
{
    struct output_buffer buffer;
    buffer.length = 0;
    buffer.capacity = text_length(text) + 32;
    buffer.data = malloc((size_t)buffer.capacity);
    if (buffer.data == NULL)
    {
        return NULL;
    }
    buffer.data[0] = '\0';

    for (int i = 0; text[i] != '\0';)
    {
        if (text[i] != '$')
        {
            if (buffer_add_character(&buffer, text[i]) < 0)
            {
                free(buffer.data);
                return NULL;
            }
            i++;
            continue;
        }

        if (text[i + 1] == '$')
        {
            if (buffer_add_character(&buffer, '$') < 0)
            {
                free(buffer.data);
                return NULL;
            }
            i += 2;
            continue;
        }

        int name_index = i + 1;
        char *name = read_variable_name(text, &name_index);
        if (name == NULL)
        {
            if (buffer_add_character(&buffer, '$') < 0)
            {
                free(buffer.data);
                return NULL;
            }
            i++;
            continue;
        }

        char *value = variable_value(name, variables, count, depth);
        free(name);
        if (value == NULL || buffer_add_text(&buffer, value) < 0)
        {
            free(value);
            free(buffer.data);
            return NULL;
        }
        free(value);
        i = name_index;
    }

    return buffer.data;
}

static int add_assignment(char *line, struct dictionnaire *variables, int count)
{
    int equal = assignment_position(line);
    if (equal < 0)
    {
        return 0;
    }

    char *name = trim_copy(line, equal);
    char *value = trim_copy(line + equal + 1, text_length(line + equal + 1));
    if (name == NULL || value == NULL || name[0] == '\0')
    {
        free(name);
        free(value);
        return -1;
    }

    char *expanded_name = expand_text(name, variables, count, 0);
    free(name);
    if (expanded_name == NULL)
    {
        free(value);
        return -1;
    }

    int result = set_variable(variables, count, expanded_name, value);
    free(expanded_name);
    free(value);
    return result;
}

int nb_de_variable_dans_le_fichier(char *fichier)
{
    if (fichier == NULL)
    {
        return 0;
    }

    int count = 0;
    int lines = nombre_de_ligne(fichier);
    char **table = transformer_fichier_en_tableau_de_ligne(fichier, lines);
    if (table == NULL)
    {
        return 0;
    }
    for (int i = 0; i < lines; i++)
    {
        if (assignment_position(table[i]) >= 0)
        {
            count++;
        }
        free(table[i]);
    }
    free(table);
    return count;
}

char *remplir_dictionnaire_variable(char *fichier, int nombre_variable)
{
    if (fichier == NULL)
    {
        return NULL;
    }

    struct dictionnaire *variables =
        calloc((size_t)nombre_variable + 1, sizeof(struct dictionnaire));
    if (variables == NULL)
    {
        return NULL;
    }

    int lines = nombre_de_ligne(fichier);
    char **table = transformer_fichier_en_tableau_de_ligne(fichier, lines);
    if (table == NULL && lines > 0)
    {
        free(variables);
        return NULL;
    }

    for (int i = 0; i < lines; i++)
    {
        if (assignment_position(table[i]) >= 0 &&
            add_assignment(table[i], variables, nombre_variable) < 0)
        {
            for (int j = 0; j < lines; j++)
            {
                free(table[j]);
            }
            free(table);
            free(variables);
            return NULL;
        }
    }

    struct output_buffer result;
    result.length = 0;
    result.capacity = text_length(fichier) + 32;
    result.data = malloc((size_t)result.capacity);
    if (result.data == NULL)
    {
        for (int i = 0; i < lines; i++)
        {
            free(table[i]);
        }
        free(table);
        free(variables);
        return NULL;
    }
    result.data[0] = '\0';

    for (int i = 0; i < lines; i++)
    {
        if (assignment_position(table[i]) < 0)
        {
            char *expanded =
                expand_text(table[i], variables, nombre_variable, 0);
            if (expanded == NULL || buffer_add_text(&result, expanded) < 0 ||
                buffer_add_character(&result, '\n') < 0)
            {
                free(expanded);
                free(result.data);
                for (int j = 0; j < lines; j++)
                {
                    free(table[j]);
                }
                free(table);
                for (int j = 0; j < nombre_variable; j++)
                {
                    free(variables[j].nom);
                    free(variables[j].valeur);
                }
                free(variables);
                return NULL;
            }
            free(expanded);
        }
        free(table[i]);
    }

    free(table);
    for (int i = 0; i < nombre_variable; i++)
    {
        free(variables[i].nom);
        free(variables[i].valeur);
    }
    free(variables);
    return result.data;
}
