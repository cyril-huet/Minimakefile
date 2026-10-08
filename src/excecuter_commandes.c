#include "minimake.h"

struct command_buffer
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

static int buffer_grow(struct command_buffer *buffer, int extra)
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

static int add_character(struct command_buffer *buffer, char character)
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

static int add_text(struct command_buffer *buffer, const char *text)
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

static int add_dependencies(struct command_buffer *buffer,
                            struct regles *target)
{
    for (int i = 0; i < target->depandance->nombre; i++)
    {
        if (i > 0 && add_character(buffer, ' ') < 0)
        {
            return -1;
        }
        if (add_text(buffer, target->depandance->var[i]) < 0)
        {
            return -1;
        }
    }
    return 0;
}

static char *replace_automatic_variables(const char *command,
                                         struct regles *target)
{
    struct command_buffer buffer;
    buffer.length = 0;
    buffer.capacity = text_length(command) + 64;
    buffer.data = malloc((size_t)buffer.capacity);
    if (buffer.data == NULL)
    {
        return NULL;
    }
    buffer.data[0] = '\0';

    for (int i = 0; command[i] != '\0'; i++)
    {
        if (command[i] != '$' || command[i + 1] == '\0')
        {
            if (add_character(&buffer, command[i]) < 0)
            {
                free(buffer.data);
                return NULL;
            }
            continue;
        }

        char automatic = command[i + 1];
        if (automatic == '@')
        {
            if (add_text(&buffer, target->target) < 0)
            {
                free(buffer.data);
                return NULL;
            }
            i++;
        }
        else if (automatic == '<')
        {
            if (target->depandance->nombre > 0 &&
                add_text(&buffer, target->depandance->var[0]) < 0)
            {
                free(buffer.data);
                return NULL;
            }
            i++;
        }
        else if (automatic == '^')
        {
            if (add_dependencies(&buffer, target) < 0)
            {
                free(buffer.data);
                return NULL;
            }
            i++;
        }
        else
        {
            if (add_character(&buffer, command[i]) < 0)
            {
                free(buffer.data);
                return NULL;
            }
        }
    }
    return buffer.data;
}

int excecuter_commande(char *commande)
{
    if (commande == NULL)
    {
        return 0;
    }

    int start = 0;
    while (commande[start] == '\t' || commande[start] == ' ')
    {
        start++;
    }
    int silent = 0;
    if (commande[start] == '@')
    {
        silent = 1;
        start++;
    }

    char *command = copy_text(commande + start);
    if (command == NULL)
    {
        return 2;
    }
    if (silent == 0)
    {
        printf("%s\n", command);
        fflush(stdout);
    }

    pid_t process = fork();
    if (process < 0)
    {
        free(command);
        return 2;
    }
    if (process == 0)
    {
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        _exit(127);
    }

    int status = 0;
    waitpid(process, &status, 0);
    free(command);
    if (WIFEXITED(status) == 0 || WEXITSTATUS(status) != 0)
    {
        fprintf(stderr, "minimake: *** Command failed. Stop.\n");
        return 2;
    }
    return 0;
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

static int already_built(struct construite *built, const char *name)
{
    while (built != NULL)
    {
        if (strcmp(built->nom, name) == 0)
        {
            return 1;
        }
        built = built->next;
    }
    return 0;
}

static int remember_target(struct construite **built, const char *name)
{
    struct construite *new_target = malloc(sizeof(struct construite));
    if (new_target == NULL)
    {
        return -1;
    }
    new_target->nom = copy_text(name);
    if (new_target->nom == NULL)
    {
        free(new_target);
        return -1;
    }
    new_target->next = *built;
    *built = new_target;
    return 0;
}

static int target_is_old(struct regles *target)
{
    struct stat target_info;
    if (stat(target->target, &target_info) != 0)
    {
        return 0;
    }
    for (int i = 0; i < target->depandance->nombre; i++)
    {
        struct stat dependency_info;
        if (stat(target->depandance->var[i], &dependency_info) != 0)
        {
            return 0;
        }
        if (dependency_info.st_mtime > target_info.st_mtime)
        {
            return 0;
        }
    }
    return 1;
}

static int build_rule(struct regles *target, struct regles **rules,
                      struct construite **built, int dependency)
{
    if (already_built(*built, target->target) == 1)
    {
        return 0;
    }
    if (remember_target(built, target->target) < 0)
    {
        return 2;
    }

    for (int i = 0; i < target->depandance->nombre; i++)
    {
        char *dependency_name = target->depandance->var[i];
        struct regles *dependency_rule = find_rule(rules, dependency_name);
        if (dependency_rule == NULL)
        {
            struct stat dependency_info;
            if (stat(dependency_name, &dependency_info) != 0)
            {
                fprintf(stderr,
                        "minimake: *** No rule to make target '%s'. Stop.\n",
                        dependency_name);
                return 2;
            }
            continue;
        }
        if (build_rule(dependency_rule, rules, built, 1) != 0)
        {
            return 2;
        }
    }

    if (target->commandes->nombre == 0)
    {
        if (dependency == 0)
        {
            printf("minimake: Nothing to be done for '%s'.\n", target->target);
        }
        return 0;
    }
    if (target_is_old(target) == 1)
    {
        if (dependency == 0)
        {
            printf("minimake: '%s' is up to date.\n", target->target);
        }
        return 0;
    }

    for (int i = 0; i < target->commandes->nombre; i++)
    {
        char *command =
            replace_automatic_variables(target->commandes->var[i], target);
        if (command == NULL)
        {
            return 2;
        }
        int result = excecuter_commande(command);
        free(command);
        if (result != 0)
        {
            return 2;
        }
    }
    return 0;
}

int tout_construire(struct regles *target, struct regles **regles_du_fichier,
                    struct construite **liste)
{
    if (target == NULL || regles_du_fichier == NULL || liste == NULL)
    {
        return 2;
    }
    return build_rule(target, regles_du_fichier, liste, 0);
}
