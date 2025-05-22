all:
	cc src/*.c -o./clide -lncurses -std=c99 -Wall -pedantic -O3

clean:
	rm -v ./clide
