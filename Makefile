# CC: compiler to use (gcc)
CC=gcc
# CFLAGS: options to pass to the compiler
CFLAGS=-c -Wall

all: myscheduler

myscheduler: scheduler.o getrand.o uniprog.o fcfs.o printing.o
	$(CC) -o scheduler scheduler.o getrand.o uniprog.o fcfs.o printing.o

scheduler.o: scheduler.c
	$(CC) $(CFLAGS) scheduler.c

uniprog.o: uniprog.c
	$(CC) $(CFLAGS) uniprog.c

getrand.o: getrand.c
	$(CC) $(CFLAGS) getrand.c
fcfs.o: fcfs.c
	$(CC) $(CFLAGS) fcfs.c
printing.o: printing.c
	$(CC) $(CFLAGS) printing.c

clean:
	rm *o scheduler