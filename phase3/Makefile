all: shell2

shell2: shell2.o
	gcc -g shell2.o -o shell2

shell2.o: shell2.h shell2.c
	gcc -g -c shell2.c

clean:
	rm -rf *.o shell2