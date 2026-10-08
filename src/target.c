#include "minimake.h"

static char *copy_text(const char *text)
{
    int length = 0;
    while (text[length] != '\0')
    {
        length++;
    }

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

static struct liste *create_list(void)
{
    struct liste *list = malloc(sizeof(struct liste));
    if (list == NULL)
    {
        return NULL;
    }
    list->var = NULL;
    list->nombre = 0;
    return list;
}

static int add_to_list(struct liste *list, const char *value)
{
    char **new_values =
        realloc(list->var, sizeof(char *) * (size_t)(list->nombre + 1));
    if (new_values == NULL)
    {
        return -1;
    }

    char *copy = copy_text(value);
    if (copy == NULL)
    {
        return -1;
    }
    list->var = new_values;
    list->var[list->nombre] = copy;
    list->nombre++;
    return 0;
}

static struct regles *create_rule(const char *name)
{
    struct regles *rule = malloc(sizeof(struct regles));
    if (rule == NULL)
    {
        return NULL;
    }

    rule->target = copy_text(name);
    rule->depandance = create_list();
    rule->commandes = create_list();
    if (rule->target == NULL || rule->depandance == NULL ||
        rule->commandes == NULL)
    {
        free(rule->target);
        free(rule->depandance);
        free(rule->commandes);
        free(rule);
        return NULL;
    }
    return rule;
}

static char *copy_part(const char *line, int start, int end)
{
    while (start < end && (line[start] == ' ' || line[start] == '\t'))
    {
        start++;
    }
    while (end > start && (line[end - 1] == ' ' || line[end - 1] == '\t'))
    {
        end--;
    }

    char *part = malloc((size_t)(end - start) + 1);
    if (part == NULL)
    {
        return NULL;
    }
    for (int i = start; i < end; i++)
    {
        part[i - start] = line[i];
    }
    part[end - start] = '\0';
    return part;
}

static int add_dependencies(struct regles *rule, const char *line, int start)
{
    int index = start;
    while (line[index] != '\0')
    {
        while (line[index] == ' ' || line[index] == '\t')
        {
            index++;
        }
        if (line[index] == '\0')
        {
            break;
        }

        int begin = index;
        while (line[index] != '\0' && line[index] != ' ' && line[index] != '\t')
        {
            index++;
        }
        char *dependency = copy_part(line, begin, index);
        if (dependency == NULL || add_to_list(rule->depandance, dependency) < 0)
        {
            free(dependency);
            return -1;
        }
        free(dependency);
    }
    return 0;
}

static int count_rules(char **lines, int line_count)
{
    int count = 0;
    for (int i = 0; i < line_count; i++)
    {
        if (lines[i][0] != '\t' && strchr(lines[i], ':') != NULL)
        {
            count++;
        }
    }
    return count;
}

static int parse_rule(char **lines, int line_count, int *line_index,
                      struct regles *rule)
{
    char *line = lines[*line_index];
    char *colon = strchr(line, ':');
    int colon_index = (int)(colon - line);
    char *name = copy_part(line, 0, colon_index);
    if (name == NULL || name[0] == '\0')
    {
        free(name);
        return -1;
    }

    free(rule->target);
    rule->target = name;
    if (add_dependencies(rule, line, colon_index + 1) < 0)
    {
        return -1;
    }

    int next_line = *line_index + 1;
    while (next_line < line_count && lines[next_line][0] == '\t')
    {
        if (add_to_list(rule->commandes, lines[next_line]) < 0)
        {
            return -1;
        }
        next_line++;
    }
    *line_index = next_line;
    return 0;
}

struct regles **tableau_regles(char **lignes, int nombre_de_ligne2)
{
    if (lignes == NULL || nombre_de_ligne2 <= 0)
    {
        return NULL;
    }

    int rule_count = count_rules(lignes, nombre_de_ligne2);
    if (rule_count == 0)
    {
        return NULL;
    }

    struct regles **rules =
        calloc((size_t)rule_count + 1, sizeof(struct regles *));
    if (rules == NULL)
    {
        return NULL;
    }

    int rule_index = 0;
    int line_index = 0;
    while (line_index < nombre_de_ligne2)
    {
        if (lignes[line_index][0] == '\t' ||
            strchr(lignes[line_index], ':') == NULL)
        {
            line_index++;
            continue;
        }

        rules[rule_index] = create_rule("");
        if (rules[rule_index] == NULL ||
            parse_rule(lignes, nombre_de_ligne2, &line_index,
                       rules[rule_index]) < 0)
        {
            free(rules[rule_index]);
            for (int i = 0; i < rule_index; i++)
            {
                free(rules[i]);
            }
            free(rules);
            return NULL;
        }
        rule_index++;
    }
    return rules;
}

static void free_list(struct liste *list)
{
    if (list == NULL)
    {
        return;
    }
    for (int i = 0; i < list->nombre; i++)
    {
        free(list->var[i]);
    }
    free(list->var);
    free(list);
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
        free_list(regles[i]->depandance);
        free_list(regles[i]->commandes);
        free(regles[i]);
    }
    free(regles);
}
