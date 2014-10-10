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

int getBurst(int n)
{
	return (1 + getRand() * n);
}

void runUni(int verbose, int NUMOFPROCS, struct process procs [] ){

	randInit();

	int time = 0;
	int ftimes[NUMOFPROCS]; // finish times
	int iotimes[NUMOFPROCS]; // io times
	int waittimes[NUMOFPROCS]; // waiting times
	int i;
	int j;
	struct process uniprocs[NUMOFPROCS];
	struct process sortprocs[NUMOFPROCS]; // used in final outputs
	int procStarts[NUMOFPROCS]; // array to keep track of sorted processes
	for(i = 0; i < NUMOFPROCS; i++){
		procStarts[i] = 0; // initialize all to "unused"
		iotimes[i] = 0; // io times start at 0
		waittimes[i] = 0; // wait times start at 0
	}
	
	// populate list with sorted values
	int countUp; // count up from 0 to number of procs of placed procs
	for(countUp = 0; countUp < NUMOFPROCS; countUp++){
		int currentMin = INT_MAX; // current minimum start time
		int minIndex = 0; // index of process with current minimum start time
		for(j = 0; j < NUMOFPROCS; j++){
			if((procStarts[j] == 0) && (procs[j].a < currentMin)){
				currentMin = procs[j].a;
				minIndex = j;
				procStarts[j] = 1; // flag for "current process has already been put in the list"
			}
		}
		uniprocs[countUp] = procs[minIndex];
		uniprocs[countUp].status = 0;
		sortprocs[countUp] = procs[minIndex];
		procStarts[minIndex] = 1;
	}
	
	int procOn = -1; // index of current running procedure
	int cpuTime = 0;
	int ioTime = 0;
	int waitTime = 0;
	
	// preprinting:
	
	printf("The original input was: %i", NUMOFPROCS);
	printprocs(NUMOFPROCS, procs);
	
	printf("The (sorted) input is:  %i", NUMOFPROCS);
	printprocs(NUMOFPROCS, uniprocs);
	
	if(verbose > 0){
		printf("\nThis detailed printout gives the state and remaining burst for each process\n\n");
	}
	
	// generate random bursts for first procs
	uniprocs[0].b = getBurst(sortprocs[0].b);
	uniprocs[0].io = getBurst(sortprocs[0].io);
	
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
		
		// update wait times for all unstarted processes
		if(time > 0){
			for(i = procOn + 1; i < NUMOFPROCS; i++){
				if(uniprocs[i].status == 1){
					waittimes[i]++;
					waitTime++;
				}
			}
		}
			
		// if start time equals time, add process
		for(i = procOn; i < NUMOFPROCS; i++){ // start at procOn
						//(process before this have obviously been added)
			if(uniprocs[i].a == time){
				uniprocs[i].status = 1;
			}
		}
	
		// assume time is left on current process (instant context switching)
		
		// if no process started, start the next ready process!
		if(procOn == -1){
			int n;
			for(n = 0; n < NUMOFPROCS; n++){
				if(uniprocs[n].status == 1){
					procOn = n;
					uniprocs[n].status = 2;
					break;
				}
			}
		}
		// check if current burst is still happening
		else if(uniprocs[procOn].b > 0){
			uniprocs[procOn].b--;
			uniprocs[procOn].c--;
			cpuTime++;
		}
		// otherwise, decrement io burst
		else{
			uniprocs[procOn].io--;
			iotimes[procOn]++;
			ioTime++;
			// if io goes to zero, reset b and io
			if(uniprocs[procOn].io == 0){
				uniprocs[procOn].b = getBurst(sortprocs[procOn].b);
				uniprocs[procOn].io = getBurst(sortprocs[procOn].io);
			}
		}
		
		// if the process went to zero time, terminate it!
		if(uniprocs[procOn].c == 0){
			uniprocs[procOn].status = 3;
			ftimes[procOn] = time;
			procOn++;
			// generate random times for first bursts
			uniprocs[procOn].b = getBurst(sortprocs[procOn].b);
			uniprocs[procOn].io = getBurst(sortprocs[procOn].io);
		}
		// that was one clock cycle, dawg (unless we're done)
		if(procOn < NUMOFPROCS){
			time++;
			
			// update status
			uniprocs[procOn].status = 2;
		}
		
	}
	
	// postprinting:
	
	int totalturnaround = 0;
	
	// print process-specific stats
	for(i = 0; i < NUMOFPROCS; i++){
		printf("\n");
		printf("Process %i:\n", i);
		printf("\t(A,B,C,IO) = (%i,%i,%i,%i)\n", sortprocs[i].a, sortprocs[i].b,\
					sortprocs[i].c, sortprocs[i].io);
		printf("\tFinishing time: %i\n", ftimes[i]);
		int turnTime = ftimes[i] - sortprocs[i].a;
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
