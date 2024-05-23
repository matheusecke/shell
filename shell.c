#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 100
#define MAX_PATHS 100

// Variável global para os caminhos de busca de executáveis
char *paths[MAX_PATHS] = { "/bin", NULL };

// Declaração das funções
void handle_cd(char *args[]);
void handle_path(char *args[]);
void handle_exit();
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
            handle_exit();
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
    int i;
    for (i = 0; i < MAX_PATHS - 1 && args[i + 1] != NULL; i++) {
        paths[i] = args[i + 1];
    }
    paths[i] = NULL;  // Terminador para indicar o final da lista
}

// Implementação do comando interno 'exit'
void handle_exit() {
    exit(0);
}

// Execução de comandos externos
void execute_external_command(char *args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        // Processo filho
        char executable[MAX_COMMAND_LENGTH];
        for (int i = 0; paths[i] != NULL; i++) {
            snprintf(executable, sizeof(executable), "%s/%s", paths[i], args[0]);
            execv(executable, args);
        }
        fprintf(stderr, "%s: command not found\n", args[0]);
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
