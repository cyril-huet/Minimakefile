#include "minimake.h"

static void print_help(void)
{
    printf("Usage: minimake [-f file] [-p] [target ...]\n");
    printf("  -f file  use another Makefile\n");
    printf("  -p       print the selected Makefile name\n");
    printf("  -h       display this help\n");
}

static void free_built_targets(struct construite *built)
{
    while (built != NULL)
    {
        struct construite *next = built->next;
        free(built->nom);
        free(built);
        built = next;
    }
}

static int parse_arguments(int argc, char **argv,
                           struct option_auxiliaire_argument *options)
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            options->tiret_h = 1;
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            options->tiret_p = 1;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr,
                        "minimake: *** missing filename after '-f'. Stop.\n");
                return 2;
            }
            i++;
            options->fichier = argv[i];
        }
    }
    return 0;
}

static char *choose_makefile(void)
{
    char *content = lire_fichier("makefile");
    if (content != NULL)
    {
        free(content);
        return "makefile";
    }

    content = lire_fichier("Makefile");
    if (content != NULL)
    {
        free(content);
        return "Makefile";
    }
    return NULL;
}

static struct regles **load_rules(const char *filename)
{
    char *content = lire_fichier((char *)filename);
    if (content == NULL)
    {
        printf("make: *** No targets specified and no makefile found.  "
               "Stop.\n");
        return NULL;
    }

    char *clean_content = nettoyage(content);
    free(content);
    if (clean_content == NULL)
    {
        return NULL;
    }

    int variable_count = nb_de_variable_dans_le_fichier(clean_content);
    char *expanded_content =
        remplir_dictionnaire_variable(clean_content, variable_count);
    free(clean_content);
    if (expanded_content == NULL)
    {
        return NULL;
    }

    int line_count = nombre_de_ligne(expanded_content);
    char **lines =
        transformer_fichier_en_tableau_de_ligne(expanded_content, line_count);
    free(expanded_content);
    if (lines == NULL)
    {
        return NULL;
    }

    struct regles **rules = tableau_regles(lines, line_count);
    for (int i = 0; i < line_count; i++)
    {
        free(lines[i]);
    }
    free(lines);
    return rules;
}

static int rule_count(struct regles **rules)
{
    int count = 0;
    while (rules[count] != NULL)
    {
        count++;
    }
    return count;
}

static struct regles *find_rule(struct regles **rules, const char *name)
{
    for (int i = 0; rules[i] != NULL; i++)
    {
        if (strcmp(rules[i]->target, name) == 0)
        {
            return rules[i];
        }
    }
    return NULL;
}

static int build_requested_targets(int argc, char **argv, struct regles **rules,
                                   struct construite **built)
{
    int status = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            i++;
            continue;
        }
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-p") == 0)
        {
            continue;
        }

        struct regles *rule = find_rule(rules, argv[i]);
        if (rule == NULL)
        {
            fprintf(stderr,
                    "minimake: *** No rule to make target '%s'. Stop.\n",
                    argv[i]);
            status = 2;
            continue;
        }
        if (tout_construire(rule, rules, built) != 0)
        {
            status = 2;
        }
    }
    return status;
}

int main(int argc, char **argv)
{
    struct option_auxiliaire_argument options = {0, 0, NULL};
    if (parse_arguments(argc, argv, &options) != 0)
    {
        return 2;
    }
    if (options.tiret_h == 1)
    {
        print_help();
        return 0;
    }

    if (options.fichier == NULL)
    {
        options.fichier = choose_makefile();
    }
    if (options.fichier == NULL)
    {
        fprintf(stderr,
                "make: *** No targets specified and no makefile found.  "
                "Stop.\n");
        printf("pas de target\n");
        return 2;
    }
    if (options.tiret_p == 1)
    {
        printf("%s\n", options.fichier);
        return 0;
    }

    struct regles **rules = load_rules(options.fichier);
    if (rules == NULL || rules[0] == NULL)
    {
        free(rules);
        printf("pas de target\n");
        return 2;
    }

    struct construite *built = NULL;
    int status;
    if (argc == 1 ||
        (argc == 3 && options.fichier != NULL && strcmp(argv[1], "-f") == 0))
    {
        status = tout_construire(rules[0], rules, &built);
    }
    else
    {
        status = build_requested_targets(argc, argv, rules, &built);
    }

    free_built_targets(built);
    liberer_regles(rules, rule_count(rules));
    return status;
}
