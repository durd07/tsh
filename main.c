#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <pthread.h>

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

int parent, child;
int exec_process_over = 0;
int sub_process_id;
int exec_pipe[2];

static char user[MAX_USER_LENGTH] = {0};
static char current_work_path[MAX_PATH_LENGTH] = {0};
static char PS1[MAX_USER_LENGTH + MAX_PATH_LENGTH] = {0};

static void* start_qt_gui(void* args);
static int exec_shell(int argc, char* argv[]);
static void usage(void);
static void version(void);
static void readopt(int argc, char *argv[]);
static void execute(char *argv[]);
static void* get_subpid(void* args);
static void cd(char *argv[]);
void handle_signal(int sig) ;

int main(int argc, char *argv[]) {
	setvbuf(stdout, 0, _IOLBF, 0);
	int fd[2]/*, child*/;
	pid_t pid;

    if(socketpair(AF_UNIX, SOCK_STREAM, 0, fd) < 0) {
//	if(pipe(fd) < 0) {
		return -1;
	}

	parent = fd[0];
	child = fd[1];

//	if((pid = fork()) < 0) {
//		close(parent);
//		close(child);
//	}
//
//	if(0 == pid) {
//		close(parent);

		if(child != STDIN_FILENO) {
			if(dup2(child, STDIN_FILENO) < 0) {
				close(child);
				return -1;
			}
        }

		if(child != STDOUT_FILENO) {
			if(dup2 (child, STDOUT_FILENO) < 0) {
				close (child);
				return -1;
			}
		}
/*
        if(child != STDERR_FILENO) {
            if(dup2 (child, STDERR_FILENO) < 0) {
                close (child);
                return -1;
            }
        }
*/
        // create the pipe to get the subprocess id
        // in order to kill it whenever it is necessary.
        if(pipe(exec_pipe) < 0) {
            perror("create exec_pipe");
            return;
        }

		pthread_t pthread_id;
		pthread_create(&pthread_id, NULL, start_qt_gui, NULL);
		
		pthread_t get_subpid_pthread_id;
//		pthread_create(&get_subpid_pthread_id, NULL, get_subpid, NULL);
		
		exec_shell(argc, argv);

		//_exit(127);
//	} else {
//		close(child);
//		start_qt_gui(NULL);
//		_exit(127);
//	}
}

void* start_qt_gui(void* args)
{
    int argc;
	char* argv[2];
	void *handle;
	void *handle_core;	
	int (*func_core)(int , char **);
	char *error;

	handle_core = dlopen("./libsimqt.so", RTLD_LAZY | RTLD_GLOBAL);
	if(!handle_core) {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	dlerror();    // Clear any existing error 
	*(void **)(&func_core) = dlsym(handle_core , "simqt_main");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}

	func_core(argc,argv);
	_exit(127);
	return NULL;
}

int exec_shell(int argc, char* argv[])
{
	//	pthread_t pthread_id;
	//	pthread_create(&pthread_id, NULL, start, NULL);
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

//			if(read_from_stdin == 0) {
            //command = readline(PS1);
//            command = readline(NULL);
            command = readline("simics# ");
            write(2, command, 7);
//			}else {
//			command = (char*)malloc(10000);
//			memset(command, 0, 10000);
//			read( child , command, 10000 );
//			}

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
				exec_process_over = 1;
				break;
			}

			if (strcmp(argv[0], "cd") == 0) {
				cd(argv);
                exec_process_over = 1;
				sub_process_id = 0;
				continue;
			}

			execute(argv);
			exec_process_over = 1;
			sub_process_id = 0;
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
		pid_t sub_pid[1];
		sub_pid[0] = getpid();

		int write_count = 0;
		if(write_count = write(exec_pipe[1], sub_pid, sizeof(pid_t)) < 0) {
			perror("write to exec_pipe");
		}
		signal(SIGINT, handle_signal);

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

void *get_subpid(void* args) {
	
	pid_t sub_id[1] = {0};

	while(1) {
		if(read(exec_pipe[0], sub_id, sizeof(pid_t) * 1) < 0) {
			perror("read exec_pipe");
			return;
		}
		sub_process_id = sub_id[0];
	}
}

void handle_signal(int sig) {
	exit(0);
}
static void cd(char *argv[]) {
	if (argv[1] != NULL) { 
		if (chdir(argv[1])  < 0) {
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
