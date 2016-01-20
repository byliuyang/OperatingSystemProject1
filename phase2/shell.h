#ifndef RUN_COMMAND_H
#define RUN_COMMAND_H

#define ENV_DELIMITER ':'
#define PATH_DELIMITER "/"
#define MAX_STR_LENGTH 2048
#define MAX_ARGUMENTS 40
#define CMD_EXIT "exit"
#define CMD_CD "cd"
#define PROMPT "==> "

struct command {
	char *name; // File name of the command
	char **params; // Parameters of the command
	int bgexec;
};

struct arginfo {
	int argc;
	char *argv[MAX_ARGUMENTS];
};

struct command get_command(struct arginfo ai);
struct arginfo getarginfo(int prompt_len, char *str);
int run_command(struct command cmd);
int get_time_elasped(struct timeval start, struct timeval end);
long get_time_by_cpu(struct timeval user, struct timeval sys);
int printstdout(int pipefd, char *buffer);
int printstati(struct rusage usage, struct timeval start_time, struct timeval end_time);

#endif 