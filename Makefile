CC = gcc
CFLAGS = -g -Wall -Werror -std=c99

all: simul

simul: simul.o elotable.o

run: simul
	@cat stage3 | ./simul

clean:
	rm -rf *.o simul
