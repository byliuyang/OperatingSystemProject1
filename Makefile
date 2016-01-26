all: runCommand shell shell2

runCommand: phase1/runCommand.o
	gcc -g phase1/runCommand.o -o runCommand

runCommand.o: phase1/runCommand.h phase1/runCommand.c
	gcc -g -c phase1/runCommand.c

shell: phase2/shell.o
	gcc -g phase2/shell.o -o shell

shell.o: phase2/shell.h phase2/shell.c
	gcc -g -c phase2/shell.c

shell2: phase3/shell2.o
	gcc -g phase3/shell2.o -o shell2

shell2.o: phase3/shell2.h phase3/shell2.c
	gcc -g -c phase3/shell2.c

clean:
	rm -rf runCommand shell shell2 phase1/*.o phase2/*.o phase3/*.o