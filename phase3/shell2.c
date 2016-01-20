#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include "shell2.h"

struct tasks ts;

int main(int argc, char *argv[]) {
  char *line;
  int listening = 1;
  struct arginfo ai;
  struct command cmd;
  int len;
  
  // Listening for command from standard input
  while (listening) {
    // Call to parse string in to conmmand line paramters
    
    // Call to get_command
    // Checkout exit, cd, if true, run in parent process; run in child process if false
    printf("%s", PROMPT);

    getline(&line, &len,stdin);

    ai = getarginfo(strlen(PROMPT), line);
    cmd = get_command(ai);

    if(!strcmp(cmd.name, CMD_EXIT)) {
    	free(line);
    	exit(0);

    } else if(!strcmp(cmd.name, CMD_CD)) {
    	if(chdir(cmd.params[1]) == -1)
    		printf("Cannot change working directory to %s\n", cmd.params[1]);
    } else if(!strcmp(cmd.name, CMD_JOBS)){
    	showactivetasks();
    } 
    else {
    	// execute(cmd, &ts);
    	execute(cmd);
    }
  }
  free(line);
}

int execute(struct command cmd){
	int pid; // Process id
	int mpid;
	int status; // Process status

	int pipefd[2]; // Pipe file descriptor
	pipe(pipefd);
	char buffer[MAX_STR_LENGTH]; // Buffer output

	struct timeval start_time;
	struct timeval end_time;

	// Resource usage statistics
	struct rusage usage;

	long time_by_cpu;

	pid = fork();
	switch(pid) {
		case -1:
			puts("Fail to create child process");
			break;
		case 0:
			// puts("Inside child process");
			close(pipefd[0]);

			// Copy output into pipe
			dup2(pipefd[1], STDOUT_FILENO);
			// Copy error messages into pipe
			dup2(pipefd[1], STDERR_FILENO);
			
			if(run_command(cmd) == 2) 
				puts("Cannot run command");
			break;
		default:

			// puts("Inside parent process");
			close(pipefd[1]);

			// Wait child process to terminate
			if((mpid = wait3(&status, WNOHANG, NULL))) {
				printf("[%d] %d \n", mpid);

				wait4(pid, &status, 0, &usage);

				// Print out 
				while(read(pipefd[0], buffer, sizeof buffer)) {
					printf("%s", buffer);
				}

				close(pipefd[0]);
				
				puts("");
				time_by_cpu = get_time_by_cpu(usage.ru_utime, usage.ru_stime);

				printf("CPU time used in microsecond:%ld\n", time_by_cpu);
				printf("# of times the process preempted involuntarily:%ld\n", usage.ru_nivcsw);
				printf("# of times the process give up CPU voluntarily:%ld\n", usage.ru_nvcsw);
				printf("# of page fault:%ld\n", usage.ru_minflt);
				printf(
					"# of page fault that could be satisfied using unreclaimed pages:%ld\n",
					usage.ru_majflt);
			}

			if(cmd.bgexec) { 

				ts.count++;
				ts.numactive++;
				ts.tasks = (struct task *)realloc(ts.tasks, ts.count * sizeof (struct task));

				ts.tasks[ts.count - 1].pid = pid;
				ts.tasks[ts.count - 1].cmdname = strdup(cmd.name);
				ts.tasks[ts.count - 1].active = 1;

				printf("[%d] %d\n", ts.count, pid);


			} else {
				// Wait child process to terminate
				wait4(pid, &status, 0, &usage);

				// Print out 
				while(read(pipefd[0], buffer, sizeof buffer)) {
					printf("%s", buffer);
				}

				close(pipefd[0]);
				
				puts("");
				time_by_cpu = get_time_by_cpu(usage.ru_utime, usage.ru_stime);

				printf("CPU time used in microsecond:%ld\n", time_by_cpu);
				printf("# of times the process preempted involuntarily:%ld\n", usage.ru_nivcsw);
				printf("# of times the process give up CPU voluntarily:%ld\n", usage.ru_nvcsw);
				printf("# of page fault:%ld\n", usage.ru_minflt);
				printf(
					"# of page fault that could be satisfied using unreclaimed pages:%ld\n",
					usage.ru_majflt);
			}
	}

	return 0;
}

/*
 * This method parse string into argument info
 * @param str The target string to parse
 * @return argument vector
 */
struct arginfo getarginfo(int prompt_len, char *str) {
	struct arginfo ai;
	/* 
	 * 1) spaces ' ': skip them if not waiting for a double quote
	 * 2) double quote '"': look for the second double quote and dont skip spaces between them
	 * 3) backslash '\': treate the following space as a space character
	 * 
	 */

	ai.argc = 0;
	int i;
	char tmp[MAX_STR_LENGTH];
	
	for (i = 0; str[i] != '\0'; i++) {
		if (!isspace(str[i])) {
		  strncat(tmp, str + i, 1);
		} else if (strlen(tmp) != 0) {
			ai.argv[ai.argc] = strdup(tmp);
			memset(tmp, '\0', sizeof tmp);
			ai.argc++;
		}
		
	}
	
	return ai;
}

/*
 * This method construct command struct from command line parameters
 * @param num_args The number of command line arguments
 * @param argv The arguments array
 * @return command struct generated
 */
struct command get_command(struct arginfo ai) {
	struct command cmd;
	int arg_len = ai.argc;
	int i;

	// Perform background execution when the last parameter is '&'
	if(!strcmp(ai.argv[ai.argc - 1], BACKGROUND_EXEC)) {
		cmd.bgexec = 1;
		arg_len--;
	} else cmd.bgexec = 0;

	// Command name is the second argument
	cmd.name = strdup(ai.argv[0]);

	// The first argument is not included 
	cmd.params = (char **)malloc(arg_len * sizeof (char*));

	// Copy arguments from argument vector to command
	for(i = 0; i < arg_len; i++) {
		cmd.params[i] = strdup(ai.argv[i]);
		free(ai.argv[i]);
	}

	// Return command struct
	return cmd;
}

/*
 * This method execute target command
 * @param command The target command struct to execute
 */
int run_command(struct command cmd) {
	// /usr/exsf:/tool/dfds:/werewr/wt
	char *env_str = getenv("PATH"); // Get user $PATH environment variable

	char *path_begin = env_str; // Beginning of path name in env string
	char *path_end; // End of path name in env string

	char path[MAX_STR_LENGTH]; // Buffer to loop over path names
	int path_len; // Length of individual path name

	

	while((path_end = strchr(path_begin, ENV_DELIMITER)) != NULL) {
		
		// Get length of path name
		path_len = path_end - path_begin;
		// Copy path name in to buffer
		strncpy(path, path_begin, path_len);
		// path/cmd_name
		strcat(path, PATH_DELIMITER);

		// /usr/bin/ls
		strcat(path, cmd.name);

		// Try to execute command
		if(!access(path, X_OK)) {
			// Execute the command
			if(execvp(path, cmd.params) == -1) {
				puts (strerror(errno));
				exit(1);
			}
		}

		// Clear path name buffer
		memset(path, '\0', MAX_STR_LENGTH);

		// Skip : in the path string
		path_begin = path_end + 1;
	}

	// Error
	return 2;
}

int showactivetasks() {
	int i;
	struct task t;

	for(i = 0; i < ts.count; i++) {
		t = ts.tasks[i];
		if(t.active) printf("[%d] %d %s\n", i, t.pid, t.cmdname);
			
	}
}

struct task findactivetaskbypid(int pid) {
	int i;
	struct task t;

	for(i = 0; i < ts.count; i++) {
		t = ts.tasks[i];
		if(t.active && (t.pid == pid)) return t;
			
	}

	return t;
}

/*
 * This method get total time used by cpu
 * @param user The user timeval struct
 * @param sys The system timeval struct
 * @return time in microsecond
 */
long get_time_by_cpu(struct timeval user, struct timeval sys) {
	return (user.tv_sec * 1000000 + user.tv_usec) + (sys.tv_sec * 1000000 + sys.tv_usec);
}