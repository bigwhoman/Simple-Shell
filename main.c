#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pwd.h>
#include "exec.h"
#include "colors.h"
#include "pq.h"

#include <regex.h>
#include <dirent.h>

#define MIN(a,b) (((a)<(b))?(a):(b))

char execs[20000][512];
int exec_size = 0;

int get_matches(char **match_array, char *input_string, const char *pattern) {
    regex_t regex;
    int reti;
    char msgbuf[100];
    regmatch_t matches[5];
    reti = regcomp(&regex, pattern, REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
    }
    int concurrent_count = 0;
    char *p = input_string;
    while (p) {
        reti = regexec(&regex, p, 2, matches, 0);
        if (!reti) {
            int i;
            for (i = 0; i < 1; i++) {
                int start = matches[i].rm_so + (p - input_string);
                int end = matches[i].rm_eo + (p - input_string);
                int len = end - start;
                char captured[len + 1];
                strncpy(captured, input_string + start, len);
                captured[len] = '\0';
                match_array[concurrent_count] = strdup(captured);
                concurrent_count++;
            }
            p += matches[0].rm_eo;
        } else if (reti == REG_NOMATCH) {
            break;
        } else {
            regerror(reti, &regex, msgbuf, sizeof(msgbuf));
            fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        }
    }
    regfree(&regex);
    return concurrent_count;
}

void parse_input(char *command) {
    char *concurrent_commands[1024];
    char *pattern = "(([-a-zA-Z0-9.]+|(\"[^\"]+\"))\\s*)+;?";
    char *pattern2 = "([-a-zA-Z0-9.]+|(\"[^\"]+\"))";
    int concurrent_count = get_matches(concurrent_commands, command, pattern);
    char *command_parts[concurrent_count][1024];
    struct command all_commands[concurrent_count];
    for (int i = 0; i < concurrent_count; ++i) {
        all_commands[i].parts = (char **) calloc(1024, sizeof(char *));
        int part_count = get_matches(command_parts[i], concurrent_commands[i], pattern2);
        all_commands[i].part_count = part_count;
        for (int j = 0; j < part_count; ++j) {
            all_commands[i].parts[j] = strdup(command_parts[i][j]);
        }
    }
    execute_commands(concurrent_count, all_commands);
    for (int i = 0; i < concurrent_count; ++i) {
        free(all_commands[i].parts);
    }
}

int is_sub(const char *str1, const char *str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if (len1 < len2) {
        return -1;
    }
    int hamming_distance = 0;
    for (int i = 0; i < len2; i++) {
        if (str1[i] != str2[i]) {
            hamming_distance++;
        }
    }
    if (hamming_distance > 1) {
        return -1;
    }
    return hamming_distance * 5 + len1 - len2;
}

char **autocomplete(const char *text, int start, int end) {
    if (start != 0 || strlen(text) < 3) {
        return NULL;
    }
    PQ *pq = (PQ *) malloc(sizeof(PQ));
    pq->queue = (PQItem **) malloc(20 * sizeof(PQItem *));
    pq->size = -1;
    for (int i = 0; i < exec_size; i++) {
        int distance = is_sub(execs[i], text);
        if (distance >= 0 && distance < 10) {
            PQItem *item = (PQItem *) malloc(sizeof(PQItem));
            item->distance = -distance;
            item->value = execs[i];
            insert(pq, item);
        }
    }
    if (pq->size < 0) {
        return NULL;
    }
    char **suggested = (char **) malloc(pq->size * sizeof(char *));
    for (int i = 0; i < pq->size; i++) {
        PQItem *min = extractMin(pq);
        suggested[i] = min->value;
    }
    return suggested;
}

int main(int argc, char **argv) {
    if (argc > 2) {
        fprintf(stderr, ANSI_COLOR_RED "Error: Too many arguments\n" ANSI_COLOR_RESET);
        return 0;
    }
    if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, ANSI_COLOR_RED "Error: File couldn't be opened\n" ANSI_COLOR_RESET);
            exit(0);
        }

        char input[1024];

        while (fgets(input, 1024, fp)) {
            if (strlen(input) > 512) {
                fprintf(stderr, ANSI_COLOR_RED "Error: Input size too large\n" ANSI_COLOR_RESET);
                continue;
            }
            parse_input(input);
        }
        fclose(fp);
        return 0;
    }
    char *path = getenv("PATH");
    char *rest = NULL;
    char *token;
    for (token = strtok_r(path, ":", &rest); token != NULL; token = strtok_r(NULL, ":", &rest)) {
        DIR *d;
        struct dirent *dir;
        d = opendir(token);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (strlen(dir->d_name) < 3) {
                    continue;
                }
                strcpy(execs[exec_size], dir->d_name);
                exec_size++;
                exec_size %= 20000;
            }
            closedir(d);
            rl_attempted_completion_function = autocomplete;
        }
    }
    char *username;
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    while (1) {
        char prompt[2048];
        char *cwd = getcwd(NULL, 0);
        sprintf(prompt, "%s%s%s@%s:%s%s%s$ ", ANSI_COLOR_BLUE, pw->pw_name, ANSI_COLOR_RESET, hostname,
                ANSI_COLOR_GREEN, cwd, ANSI_COLOR_RESET);
        char *input = readline(prompt);

        if (!input) {
            break;
        }
        if (strlen(input) > 512) {
            fprintf(stderr, ANSI_COLOR_RED "Error: Input size too large\n" ANSI_COLOR_RESET);
            continue;
        }

        if (strcmp(input, "quit") == 0) {
            printf(ANSI_COLOR_YELLOW "Exiting Shell...\n" ANSI_COLOR_RESET);
            exit(0);
        }
        add_history(input);
        parse_input(input);

        free(input);
        free(cwd);
    }
    return 0;
}