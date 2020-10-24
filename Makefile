CWD = $(shell pwd)
CFLAGS = -Wall -std=c99
DBFLAG = -g
SRC = ${CWD}/src/main.c

.PHONY: clean all

all: main

${OBJ}: ${SRC} ${DEP}
	gcc $(CFLAGS) $(DBFLAG) -c $< -o $@

main: ${OBJ}
	gcc $(SRC) $(CFLAGS) $(DBFLAG) -o makeutil

clean:
	rm -f makeutil *.o
