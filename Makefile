# CC: compiler to use (gcc)
CC=gcc
# CFLAGS: options to pass to the compiler
CFLAGS=-c -Wall

all: myscheduler

myscheduler: scheduler.o getrand.o
	$(CC) -o scheduler scheduler.o getrand.o

scheduler.o: scheduler.c
	$(CC) $(CFLAGS) scheduler.c

getrand.o: getrand.c
	$(CC) $(CFLAGS) getrand.c

clean:
	rm -rm *o myscheduler