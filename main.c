#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <readline/readline.h>
#include <readline/history.h>


#define VERSION                                 \
    "tsh version 1.2.0\n"

#define USAGE                                           \
    "Usage: tsh [GNU long option] [option]... \n \
      tsh [GNU long option] [option] script-file ...\n \
      long option:\n \
          --version\n \
          --help\n"

#define MAX_PATH_LENGTH    1024
#define MAX_COMMAND_LENGTH 1024
#define MAX_USER_LENGTH    128

static struct option longoptions [] = {
    {"version", 0, NULL, 'v'},
    {"help", 0, NULL, 'h'}
};

static char user[MAX_USER_LENGTH] = {0};
static char current_work_path[MAX_PATH_LENGTH] = {0};
static char PS1[MAX_USER_LENGTH + MAX_PATH_LENGTH] = {0};

static void usage(void);
static void version(void);
static void readopt(int argc, char *argv[]);
static void execute(char *argv[]);
static void cd(char *argv[]);

int main(int argc, char *argv[]) {
    char *delimiter_position = NULL;
    char *command = NULL;
    int i = 0;
    if (argc < 2) {
        while(1) {
            if (getcwd(current_work_path, sizeof(current_work_path)) == NULL) {
                break;
            }
            if (getlogin_r(user, sizeof(user)) != 0) {
                break;
            }

            sprintf(PS1, "%s:%s$ ", user, current_work_path);

            if (command != NULL) {
                free(command);
                command = NULL;
            }
            
            command = readline(PS1);
            if ((command != NULL) && (*command != '\0')) {
                add_history(command);
            }
            
            delimiter_position = strtok(command, " ");
            while (delimiter_position != NULL) {
                argv[i++] = delimiter_position;
                delimiter_position = strtok(NULL, " ");
            }
            argv[i] = NULL;
            i = 0;
            
            if (strcmp(argv[0], "quit") == 0) {
                break;
            }
            
            if (strcmp(argv[0], "cd") == 0) {
                cd(argv);
                continue;
            }

            execute(argv);
        }
    } else {
        readopt(argc, argv);
    }

    
    return 0;
}

static void usage(void) {
    fprintf(stdout, USAGE);
}

static void version(void) {
    fprintf(stdout, VERSION);
}

static void readopt(int argc, char *argv[]) {
    int opt = 0;
    while ((opt = getopt_long(argc, argv, "vh", longoptions, NULL)) != EOF) {
        switch (opt) {
            case 'h':
                usage();
                break;
            case 'v':
                version();
                break;
            default:
                usage();
                break;
        }
    }
}

static void execute(char *argv[]) {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        fprintf(stdout, "SOME HAPPENED IN FORK!\n");
        exit(2);
    } else if (pid == 0) {
        if (execvp(argv[0], argv) < 0) {
            switch (errno) {
                case ENOENT:
                    fprintf(stdout, "COMMAND OR FILENAME NOT FOUND\n");
                    break;
                case EACCES:
                    fprintf(stdout, "YOU DO NOT HAVE RIGHT TO ACCESS!\n");
                    break;
                default:
                    fprintf(stdout, "SOME ERROR HAPPENED IN EXEC\n");
            }
        }
        exit(3);
    } else {
        wait(NULL);
    }
}

static void cd(char *argv[]) {
    if (argv[1] != NULL) {
        if (chdir(argv[1]) < 0) {
            switch (errno) {
                case ENOENT:
                    fprintf(stdout, "DIRECTORY NOT FOUND!\n");
                    break;
                case ENOTDIR:
                    fprintf(stdout, "Not A DIRECTORY NAME!\n");
                    break;
                case EACCES:
                    fprintf(stdout, "YOU DO NOT HAVE RIGHT TO ACCESS!\n");
                    break;
                default:
                    fprintf(stdout, "SOME ERROR HAPPEND IN CHDIR!\n");
                    break;
            }
        }
    }
}
