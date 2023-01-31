#ifndef SIMPLE_SHELL_EXEC_H
#define SIMPLE_SHELL_EXEC_H

#endif //SIMPLE_SHELL_EXEC_H

struct command {
    // Parts is basically the parts of the command to run.
    // For example parts might be allocated like {"echo", "hello", "world"}
    char **parts;
    // Part count is the number of parts in parts field
    int part_count;
};

/**
 * Executes multiple commands simultaneously. Waits for all of them to exit and then
 * returns from function.
 * @param command_count Number of commands to run.
 * @param commands List of commands.
 */
void execute_commands(int command_count, const struct command *commands);