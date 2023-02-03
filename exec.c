#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include "colors.h"
#include "exec.h"

void execute_commands(int command_count, const struct command *commands) {
    int ran_commands = 0;
    // Loop for each command
    for (int i = 0; i < command_count; i++) {
        // Check for zero parts
        if (commands[i].part_count == 0) {
            // We silently ignore these. These cases can be created with lines like ";     ;     ;"
            // fprintf(stderr, ANSI_COLOR_RED "Error: Empty command.\n" ANSI_COLOR_RESET);
            continue;
        }
        // Check for CD command
        if (strcmp(commands[i].parts[0], "cd") == 0) {
            // cd must have at least one argument
            if (commands[i].part_count < 2) {
                fprintf(stderr, ANSI_COLOR_RED
                                "Please supply the path to cd.\n"
                                ANSI_COLOR_RESET);
                continue;
            }
            // Change the directory
            if (chdir(commands[i].parts[1]) != 0)
                fprintf(stderr, ANSI_COLOR_RED
                                "Error: Failed to cd to %s: %s\n"
                                ANSI_COLOR_RESET,
                        commands[i].parts[1], strerror(errno));
            continue;
        }

        // Fork and execute in child
        int fork_result = fork();
        if (fork_result < 0) {
            fprintf(stderr, ANSI_COLOR_RED "Error: Failed to fork process: %s\n" ANSI_COLOR_RESET, strerror(errno));
        } else if (fork_result == 0) { // child process
            // Note: I think stack allocating this is a good move.
            // No more free/malloc bullshit. Also, we are just pointers. Each take 8 bytes.
            // So probably no stackoverflow either.
            char *execvp_args[commands[i].part_count + 1];
            for (int j = 0; j < commands[i].part_count; j++)
                execvp_args[j] = commands[i].parts[j];

            execvp_args[commands[i].part_count] = NULL; // last one should be null
            // Run the command
            if (execvp(execvp_args[0], execvp_args) < 0) {
                fprintf(stderr, ANSI_COLOR_RED
                                "Error: Failed to execute command %s: %s\n"
                                ANSI_COLOR_RESET,
                        execvp_args[0], strerror(errno));
            }
            _exit(1);
        } else { // parent and forked
            ran_commands++;
        }
    }
    // Wait for all child processes
    for (int i = 0; i < ran_commands; i++) {
        int status;
        wait(&status);
    }
}