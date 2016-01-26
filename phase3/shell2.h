#ifndef RUN_COMMAND_H
#define RUN_COMMAND_H

#define ENV_DELIMITER ':'
#define PATH_DELIMITER "/"
#define MAX_STR_LENGTH 2048
#define MAX_ARGUMENTS 40

#define CMD_EXIT "exit"
#define CMD_CD "cd"
#define CMD_JOBS "jobs"

#define PROMPT "==> "
#define BACKGROUND_EXEC "&"

struct command {
	int bgexec;
	char *name; // File name of the command
	char **params; // Parameters of the command
};

struct arginfo {
	int argc;
	char *argv[MAX_ARGUMENTS];
};

struct tasks {
	int count;
	int numactive;
	struct task *tasks;
};

struct task {
	int tid;
	int pid;
	int active;
	char *cmdname;
};

int execute(struct command cmd);
struct command get_command(struct arginfo ai);
struct arginfo getarginfo(int prompt_len, char *str);
int run_command(struct command cmd);
long get_time_by_cpu(struct timeval user, struct timeval sys);
void printstat(struct rusage usage);
int showactivetasks();
int findactivetaskbypid(int pid);
void waitForChild(int options);
void freetasks();
#endif 