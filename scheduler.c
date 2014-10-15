// Jason Angell
// Main scheduling file for lab 2
// Operating Systems

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "myrand.h" // random number generator
#include "processStruct.h" // struct process {a, b, c, io, status} & linked list
#include "uniprog.h" // uniprogramming
#include "fcfs.h" // fcfs

// size function to determine the length of arrays
#define SIZE(x) (sizeof(x)/sizeof(x[0]))

// prints linked list in order (primarily for debugging
void printList(struct listNode curNode)
{
	int itemNum = 0;
	while(curNode.next != NULL){
		printf("%i:\tval = %i\tnext = %p\n", itemNum, curNode.val, curNode.next);
		curNode = *(curNode.next);
	}
	printf("%i:\tval = %i\tnext = %p\n", itemNum, curNode.val, curNode.next);
}

// scans through file until it finds an int, then returns it
// *** not functional for ints of more than one digit ***
int getNextInt(FILE * fp)
{
  char cur;
  while(! isdigit(cur = fgetc(fp))){} // iterates until it finds the next int
  int curInt = (cur - '0');
  while(isdigit(cur = fgetc(fp))){
		curInt = (curInt * 10) + (cur - '0');
	}
  return (curInt);
}

// gets verbosity argument (verbose or show-rand)
// returns int based on argument (verbose=1, show-rand=2)
// returns 0 and prints error for invalid argument
int getVerb(char arg[])
{
	int toRet = 0;
	if(strncmp(arg, "--verbose", 10) == 0){
      toRet = 1;
    }
    else if(strncmp(arg, "--show-random", 13) == 0){
			toRet = 2;
    }
    else{
      printf("Illegal argument given: %s\n", arg);
      printf("Valid arguments are --verbose and --show-random\n");
      printf("Proceeding in non-verbose mode\n");
    }
    return toRet;
}

// command line argument(s) should be verbosity (optional) and input file
int main(int argv, char * argc[])
{
  
  int verbose = 0; // 0 = non-verbose, 1 = verbose, 2 = show-rand
  FILE * fp = 0;
  
  int numArgs = argv;
  
  // get file pointer and verbosity argument
  if(numArgs == 2){
    fp = fopen(argc[1], "r");
  }
  else if(numArgs == 3){
    verbose = getVerb(argc[1]);
    fp = fopen(argc[2], "r");
  }
  // check for invalid file
  if(fp == NULL){
    printf("FATAL ERROR: invalid file given\n");
    exit(1);
  }

  char nextChar = ' ';
  int NUMOFPROCS = 0;
  while(NUMOFPROCS == 0){
    nextChar = fgetc(fp);
    if(isdigit(nextChar)){
      NUMOFPROCS = nextChar - '0'; // subtracts the ascii value for 0 from int value
    }
  }

  int i = 0;
  struct process procs[NUMOFPROCS];
  
  while(i < NUMOFPROCS){
    procs[i].a = getNextInt(fp);
    procs[i].b = getNextInt(fp);
    procs[i].c = getNextInt(fp);
    procs[i].io = getNextInt(fp);
    i++;
  }
  
  fclose(fp);
  
  // sort processes by order of start time
  
  struct process sortedprocs[NUMOFPROCS];
  int sortflags[NUMOFPROCS]; // array to flag which processes have been sorted by time
  for(i = 0; i < NUMOFPROCS; i++){
  	sortflags[i] = 0; // set all to zero
  }
  int sortcount;
  int j;
  for(sortcount = 0; sortcount < NUMOFPROCS; sortcount++){
  	int minind;
  	int mintime = INT_MAX;
  	for(j = 0; j < NUMOFPROCS; j++){ // cycle through, checking each start against min
			if(procs[j].a < mintime && sortflags[j] == 0){
				minind = j;
				mintime = procs[j].a;
			}
  	}
  	// add it to the appropriate place in the sortedprocs array
  	sortedprocs[sortcount] = procs[minind];
  	sortflags[minind] = 1;
  }
  
  // ****** DO NOT EDIT PROCS OR SORTEDPROCS ******
  // actual scheduling here
  
	runUni(verbose, NUMOFPROCS, &procs, &sortedprocs);
	runfcfs(verbose, NUMOFPROCS, &procs, &sortedprocs);
	
  
}
