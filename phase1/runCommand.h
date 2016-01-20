#ifndef RUN_COMMAND_H
#define RUN_COMMAND_H

#define ENV_DELIMITER ':'
#define PATH_DELIMITER "/"
#define MAX_STR_LENGTH 1024

struct command {
	char *name; // File name of the command
	char **params; // Parameters of the command
};

struct command get_command(int argc, char *argv[]);
int run_command(struct command cmd);
int get_time_elasped(struct timeval start, struct timeval end);
long get_time_by_cpu(struct timeval user, struct timeval sys);

#endif 