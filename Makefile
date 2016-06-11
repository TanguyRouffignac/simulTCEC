CC = gcc
CFLAGS = -g -Wall -Werror -std=c99

all: simul

simul: simul.o elotable.o

run: simul
	@cat stage1b | ./simul

clean:
	rm -rf *.o simul
