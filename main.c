#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "exec.h"
#include "colors.h"

void execute_command(char *command) {
    char *argv[1024];
    int argc = 0;

    char *token = strtok(command, " ");
    while (token) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    argv[argc] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
        // child process
        if (execvp(argv[0], argv) < 0) {
            fprintf(stderr, ANSI_COLOR_RED "Error: Failed to execute command\n" ANSI_COLOR_RESET);
        }
        exit(0);
    } else if (pid > 0) {
        // parent process
        int status;
        wait(&status);
    } else {
        fprintf(stderr, ANSI_COLOR_RED "Error: Failed to fork process\n" ANSI_COLOR_RESET);
    }
}

int main(void) {
    char *input;

    while (1) {
        input = readline(ANSI_COLOR_GREEN "$ " ANSI_COLOR_RESET);

        if (!input) {
            break;
        }

        if (strcmp(input, "quit") == 0) {
            printf(ANSI_COLOR_YELLOW "Exiting Shell...\n" ANSI_COLOR_RESET);
            exit(0);
        }

        execute_command(input);

        free(input);
    }

    return 0;
}