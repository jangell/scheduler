#include <stdio.h>
#include <stdlib.h>
#include "processStruct.h" // {a, b, c, io, status}
#include "uniprog.h"

void runUni(int NUMOFPROCS, process [] procs){

	int time = 0;
	struct process uniprocs[NUMOFPROCS];
	int procStarts[NUMOFPROCS]; // array to sort processes by start time
	// sort procs into procStarts
	
	struct listNode head;
	head.val = // work on this. i'm not sure how to sort these as of yet
	head.next = NULL;
	
	// populate list with sorted values
	for(i = 1; i < NUMOFPROCS; i++){
		
	}
	
	// copy over linkedlist (head) into uniprocs
	for(i = 0; i < NUMOFPROCS; i++){
		struct listNode curNode = head;
		int countProc;
		for(countProc = 0; countProc < NUMOFPROCS; countProc++){
			curNode = *(curNode.next);
		}
		uniprocs[i] = curNode.val;
		uniprocs[i].status = 0;
	}
	
	for(i = 0; i < NUMOFPROCS; i++){
		printf("%i %i %i %i\n", uniprocs[i].a, uniprocs[i].b, uniprocs[i].c, uniprocs[i].io);
	}
	
	int procOn = 0; // index of current running procedure
	int procRun = 0; // flag for whether a running process exists
	int cpuTime = 0;
	int ioTime = 0;
	while(procOn < NUMOFPROCS){
	
		// print verbosity stuff
		if(verbose == 1){
			printf("Before cycle\t%i:", time);
			for(i = 0; i < NUMOFPROCS; i++){
				int stat = uniprocs[i].status;
				char statString[20];
				int statVal;
				if(stat == 0){
				  strcpy(statString, "unstarted");
					statVal = 0;
				}
				else if(stat == 1){
					strcpy(statString, "ready");
					statVal = 0;
				}
				else if(stat == 2){
					if(uniprocs[procOn].b != 0){
						strcpy(statString, "running");
						statVal = uniprocs[procOn].b;
					}
					else{
						strcpy(statString, "blocked");
						statVal = uniprocs[procOn].io;
					}
				}
				else if(stat == 3){
					strcpy(statString, "terminated");
					statVal = 0;
				}
				else{
					strcpy(statString, "ERROR: stat > 3");
					statVal = -1;
				}
				printf("\t%s  %i", statString, statVal);
			}
			printf("\n");
		}
		// end verbosity stuff
			
		// if start time equals time, add process
		for(i = procOn; i < NUMOFPROCS; i++){ // start at procOn (process before this have obviously been added)
			if(procs[i].a == time){
				uniprocs[i].status = 1;
				// no running process? why not this one!
				if(procRun == 0){
					procOn = i;
					uniprocs[procOn].status = 2;
					procRun = 1;
				}
			}
		}
	
		// assume time is left on current process (instant context switching)
		
		// check if current burst is still happening
		if(uniprocs[procOn].b > 0){
			uniprocs[procOn].b--;
			uniprocs[procOn].c--;
			cpuTime++;
		}
		// otherwise, decrement io burst
		else{
			uniprocs[procOn].io--;
			ioTime++;
			// if io goes to zero, reset b and io
			if(uniprocs[procOn].io == 0){
				uniprocs[procOn].b = procs[procOn].b;
				uniprocs[procOn].io = procs[procOn].io;
			}
		}
		// if the process went to zero time, terminate it!
		if(uniprocs[procOn].c == 0){
			uniprocs[procOn].status = 3;
			procOn++;
		}
		// that was one clock cycle, dawg
		time++;
		
		// update status
		uniprocs[procOn].status = 2;
		
	}

}