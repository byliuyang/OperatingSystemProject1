all: runCommand

runCommand: runCommand.o
	gcc -g runCommand.o -o runCommand

runCommand.o: runCommand.h runCommand.c
	gcc -g -c runCommand.c

clean:
	rm -rf *.o runCommand