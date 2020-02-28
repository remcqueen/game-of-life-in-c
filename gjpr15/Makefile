all: gameoflife.o gol.o gol.h libgol.so
	gcc -Wall -Wextra -pedantic -std=c11 -o gameoflife gameoflife.c -lgol -L.

gol.o: gol.c gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -c gol.c -fPIC

libgol.so: gol.o gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -shared gol.o -o libgol.so

gameoflife.o: gameoflife.c gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -c gameoflife.c

clean:
	rm -rf gameoflife gameoflife.o gol.o libgol.so