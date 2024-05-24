#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 100

// Declaração das funções
void handle_cd(char *args[]);
void handle_path(char *args[]);
void execute_external_command(char *args[]);

int main(int argc, char *argv[]) {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    // Modo batch
    FILE *input = stdin;
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    } else if (argc > 2) {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Loop principal
    while (1) {
        if (input == stdin) {
            printf("shell> ");
        }

        if ((nread = getline(&line, &len, input)) == -1) {
            if (feof(input)) {
                break;
            } else {
                perror("getline");
                continue;
            }
        }

        // Remover o newline do final
        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }

        // Dividir a linha em argumentos
        int arg_count = 0;
        char *token = strtok(line, " ");
        while (token != NULL) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        if (arg_count == 0) {
            continue;
        }

        // Comando interno ou externo
        if (strcmp(args[0], "exit") == 0) {
            break;
        } else if (strcmp(args[0], "cd") == 0) {
            handle_cd(args);
        } else if (strcmp(args[0], "path") == 0) {
            handle_path(args);
        } else {
            execute_external_command(args);
        }
    }

    free(line);

    if (input != stdin) {
        fclose(input);
    }

    return 0;
}

// Implementação do comando interno 'cd'
void handle_cd(char *args[]) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: expected argument\n");
    } else if (chdir(args[1]) != 0) {
        perror("cd");
    }
}

// Implementação do comando interno 'path'
void handle_path(char *args[]) {
    fprintf(stderr, "path: command not supported in this version\n");
}

// Execução de comandos externos
void execute_external_command(char *args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        // Processo filho
        execvp(args[0], args);
        // Se execvp retornar, houve um erro
        perror(args[0]);
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        // Processo pai
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Program exited with status %d\n", WEXITSTATUS(status));
        }
    }
}
