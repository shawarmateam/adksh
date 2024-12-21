//#include <adklib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* system_str(const char* cmd) {
    FILE *fp;
    char buffer[1035];
    char *output = NULL;
    size_t output_size = 0;

    fp = popen(cmd, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t buffer_length = strlen(buffer);
        char *new_output = realloc(output, output_size + buffer_length + 1);
        if (new_output == NULL) {
            free(output);
            printf("Memory allocation failed\n");
            exit(1);
        }
        output = new_output;
        strcpy(output + output_size, buffer);
        output_size += buffer_length;
    }

    pclose(fp);
    output[output_size-1] = '\0';
    return output;
}

#define PREFIX printf("%s - %s-> ", getenv("USER"), system_str("fish -c \"pwd | sed 's|^$HOME|~|'\""))

char *read_line()
{
    char *line = 0;
    size_t len = 0;
    ssize_t read;

    read = getline(&line, &len, stdin);
    if (read == -1)
    {
        if (feof(stdin))
        {
            free(line);
            puts("\nEOF (^D)");
            exit(0);
        } else
        {
            perror("getline");
            exit(-1);
        }
    }

    return line;
}

char **split_line(char *line)
{
    line[strcspn(line, "\n")] = 0;

    char **tokens = malloc(64 * sizeof(char*));
    if (!tokens) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int position = 0;
    char *token = strtok(line, " ");
    while (token != NULL) {
        tokens[position++] = token;
        token = strtok(NULL, " ");
    }
    tokens[position] = NULL;

    return tokens;
}

int execute(char ** args)
{
    return system(args[0]);
}

void loop(void)
{
    char * line;
    char **args;
    int status;

    while (1)
    {
        PREFIX;
        line = read_line();
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    }
}

int main() {
    loop();
    return 0;
}
