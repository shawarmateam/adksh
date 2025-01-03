//#include <adklib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char* system_str(const char* cmd) {
    FILE *fp;
    char buffer[1035];
    char *output = NULL;
    size_t output_size = 0;

    fp = popen(cmd, "r");
    if (fp == NULL) {
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t buffer_length = strlen(buffer);
        char *new_output = realloc(output, output_size + buffer_length + 1);
        if (new_output == NULL) {
            free(output);
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

char **split_line(char *line, int *count)
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
    *count = position;

    return tokens;
}

int check_builtin(char ** args)
{
    if (0==strcmp(args[0],"exit"))
    {
        exit(0);
    }

    if (0==strcmp(args[0],"help"))
    {
        printf("adksh. - adk shell. simply shell in C\n");
        return 1;
    }

    if (0==strcmp(args[0],"cd"))
    {
        if (args[1]==0) args[1]=getenv("HOME");
        chdir(args[1]);
        return 1;
    }

    return 0;
}


int execute(char ** args, int len)
{
    if (check_builtin(args)) return 0;

    pid_t pid, wpid;
    int status;
    char isFG = 0x00;
    if (0==strcmp(args[len-1],"&"))
    {
        isFG=0xff;
        args[len-1]=0;
    }

    pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1) perror("adksh");
        exit(EXIT_FAILURE);
    }

    else if (pid < 0)
    {
        perror("adksh");
    }

    else if (!isFG)
    {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        }

        while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
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
        if (line[0]==0 || line[0]=='\n') continue;

        int len=0;
        args = split_line(line, &len);
        status = execute(args, len);

        free(line);
        free(args);
    }
}

int main(int argc, char ** argv) {
    if (argc==1)
    loop();

    else if (argc==2)
    {
        if (0==strcmp(argv[1],"-h"))
        {
            char * helpcmd[] = {"help", 0};
            return execute(helpcmd, 1);
        }
    }

    else if (argc==3)
    {
        if (0==strcmp(argv[1],"-c"))
        {
            int len=0;
            char ** args = split_line(argv[2], &len);
            return execute(args, len);
        }
    }

    return 0;
}
