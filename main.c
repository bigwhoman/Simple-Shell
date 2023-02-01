#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "exec.h"
#include "colors.h"
#include <pwd.h>

char *getcwd(char *buf, size_t size);

void parse_input(char *command) {
    struct command all_commands[1024];
    char *argv[1024];
    int argc = 0;
    char *saveptr1, *saveptr2;
    char *concurrent_command = strtok_r(command, ";", &saveptr1);
    while (concurrent_command) {
        argv[argc++] = concurrent_command;

        char *part[2000];
        int part_num = 0;
        char copy[1024];

        strcpy(copy, concurrent_command);
        all_commands[argc].parts = (char **) calloc(1024, sizeof(char *));
        char *command_part = strtok_r(copy, " ", &saveptr2);
        while (command_part) {
            all_commands[argc].parts[part_num] = command_part;
            part[part_num++] = command_part;
            command_part = strtok_r(NULL, " ", &saveptr2);
        }
        all_commands[argc].part_count = part_num;
        part[part_num] = NULL;
        concurrent_command = strtok_r(NULL, ";", &saveptr1);
    }
    argv[argc] = NULL;
    execute_commands(argc, all_commands);
}


int main(void) {
    char *input;
    char cwd[1024];
    char *username;
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    char hostname[256];
    int ret = gethostname(hostname, sizeof(hostname));
    while (1) {
        char prompt[2048];
        char *cwd = getcwd(NULL, 0);
        sprintf(prompt, "%s%s%s@%s %s~%s%s$ %s", ANSI_COLOR_BLUE, pw->pw_name, ANSI_COLOR_RESET, hostname,
                ANSI_COLOR_GREEN, cwd, ANSI_COLOR_RESET, ANSI_COLOR_CYAN);
        char *input = readline(prompt);

        if (!input) {
            break;
        }

        if (strcmp(input, "quit") == 0) {
            printf(ANSI_COLOR_YELLOW "Exiting Shell...\n" ANSI_COLOR_RESET);
            exit(0);
        }
        parse_input(input);

        free(input);
        free(cwd);
    }

    return 0;
}