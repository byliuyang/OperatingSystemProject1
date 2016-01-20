all: shell

shell: shell.o
	gcc -g shell.o -o shell

shell.o: shell.h shell.c
	gcc -g -c shell.c

clean:
	rm -rf *.o shell