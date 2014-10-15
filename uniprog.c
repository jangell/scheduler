#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "myrand.h"
#include "processStruct.h" // {a, b, c, io, status}
#include "uniprog.h" // uni header

// message must come before this describing sorted / unsorted
void printprocs(int numProcs, struct process procs [])
{
	int count;
	for(count = 0; count < numProcs; count++){
		printf(" ( %i %i %i %i )", procs[count].a, procs[count].b, procs[count].c,\
					procs[count].io);
	}
	printf("\n");
}

int getBurst(int n, int verbose)
{
	int rand = getRand();
	if(verbose == 2){
		printf("Find burst when choosing ready process to run %i\n", rand);
	}
	return (1 + rand%n);
}

void runUni(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs []){

	randInit();

	// create simulated "process table" with arrays and values
	int time = 0; // running time
	int ftimes[NUMOFPROCS]; // finish times
	int iotimes[NUMOFPROCS]; // io times
	int waittimes[NUMOFPROCS]; // waiting times
	// struct of processes on which the scheduler is based
	struct process uniprocs[NUMOFPROCS];
	
	// counting variables
	int i;
	int j;
	
	// copy sprocs over into another (mutable) array
	for(i = 0; i < NUMOFPROCS; i++){
		uniprocs[i] = sprocs[i];
	}
	
	uniprocs[0].a--;
	printf("subtracted one from uniprocs\nsprocs.a: %i\n", sprocs[0].a);
	printf("uniprocs.a: %i\n", uniprocs[0].a);
	
	
	int procStarts[NUMOFPROCS]; // array to keep track of sorted processes
	for(i = 0; i < NUMOFPROCS; i++){
		procStarts[i] = 0; // initialize all to "unused"
		iotimes[i] = 0; // io times start at 0
		waittimes[i] = 0; // wait times start at 0
	}
	
	int procOn = 0; // index of current running procedure
	int cpuTime = 0;
	int ioTime = 0;
	int waitTime = 0;
	
	// preprinting:
	
	printf("The original input was: %i", NUMOFPROCS);
	printprocs(NUMOFPROCS, procs);
	
	printf("The (sorted) input is:  %i", NUMOFPROCS);
	printprocs(NUMOFPROCS, sprocs);
	
	if(verbose > 0){
		printf("\nThis detailed printout gives the state and remaining burst for each process\n\n");
	}
	
	int nextBurst;
	int task; // flag: 0 = cpu burst, 1 = io burst
	int cputogo = 0;
	struct process curproc;
	
	while(procOn < NUMOFPROCS){
	
		// print verbosity stuff
		if(verbose > 0){
			printf("Before cycle");
			printf("%6i:", time);
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
				printf("\t%12s%3i", statString, statVal);
			}
			printf(".\n");
		}
		// end verbosity stuff
		
		// let's do this
		nextBurst = 0;
		task = 0;
		curproc = uniprocs[procOn];
		if(curproc.a > time){
			cputogo = curproc.c; // processing time left on the current task
		}
		
		
		// scheduling for a single process here
		while(cputogo > 0){
			if(task == 0){
				nextBurst = getBurst(curproc.b, verbose);
			}
			else if(task == 1){
				nextBurst = getBurst(curproc.io, verbose);
			}
			while(nextBurst > 0){
				
				// verbosity should go here
				
				
				
			}
		}
		
		
	}
	
	// postprinting:
	
	int totalturnaround = 0;
	
	// print process-specific stats
	for(i = 0; i < NUMOFPROCS; i++){
		printf("\n");
		printf("Process %i:\n", i);
		printf("\t(A,B,C,IO) = (%i,%i,%i,%i)\n", uniprocs[i].a, uniprocs[i].b,\
					uniprocs[i].c, uniprocs[i].io);
		printf("\tFinishing time: %i\n", ftimes[i]);
		int turnTime = ftimes[i] - uniprocs[i].a;
		totalturnaround += turnTime;
		printf("\tTurnaround time: %i\n", turnTime);
		printf("\tI/O time: %i\n", iotimes[i]);
		printf("\tWaiting time: %i\n", waittimes[i]);
	}
	
	// print general summary
	
	printf("\n");
	printf("Summary Data:\n");
	printf("\tFinishing time: %i\n", time);
	float cputil = (float) cpuTime / time;
	printf("\tCPU Utilization: %6f\n", cputil);
	float ioutil = (float) ioTime / time;
	printf("\tI/O Utilization: %6f\n", ioutil);
	float thru = (float) NUMOFPROCS * (100. / time);
	printf("\tThroughput: %.6f processes per hundred cycles\n", thru);
	float turnavg = (float) totalturnaround / NUMOFPROCS;
	printf("\tAverage turnaround time: %.6f\n", turnavg);
	float waitavg = (float) waitTime / NUMOFPROCS;
	printf("\tAverage waiting time: %.6f\n", waitavg);
	
}
