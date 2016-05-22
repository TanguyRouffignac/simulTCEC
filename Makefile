CFLAGS = -g -Wall -Werror -std=c99

all: simul

simul: simul.o elotable.o

run: simul
	@cat input | ./simul

clean:
	rm -rf *.o simul
