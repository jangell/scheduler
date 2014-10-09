#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
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

void runUni(int verbose, int NUMOFPROCS, struct process procs [] ){

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
				procStarts[j] = 1;
			}
		}
		uniprocs[countUp] = procs[minIndex];
		uniprocs[countUp].status = 0;
		sortprocs[countUp] = procs[minIndex];
		procStarts[minIndex] = 1;
	}
	
	int procOn = 0; // index of current running procedure
	int procRun = 0; // flag for whether a running process exists
	int cpuTime = 0;
	int ioTime = 0;
	int waitTime = 0;
	
	// preprinting:
	
	printf("The original input was: %i", NUMOFPROCS);
	printprocs(NUMOFPROCS, procs);
	
	printf("The (sorted) input is:  %i", NUMOFPROCS);
	printprocs(NUMOFPROCS, uniprocs);
	
	printf("\nThis detailed printout gives the state and remaining burst for each process\n\n");
	
	while(procOn < NUMOFPROCS){
	
		// print verbosity stuff
		if(verbose == 1){
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
				printf("\t%14s%3i", statString, statVal);
			}
			printf(".\n");
		}
		// end verbosity stuff
			
		// if start time equals time, add process
		for(i = procOn; i < NUMOFPROCS; i++){ // start at procOn
						//(process before this have obviously been added)
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
			iotimes[procOn]++;
			ioTime++;
			// if io goes to zero, reset b and io
			if(uniprocs[procOn].io == 0){
				uniprocs[procOn].b = procs[procOn].b;
				uniprocs[procOn].io = procs[procOn].io;
			}
		}
		
		// update wait times for all unstarted processes
		for(i = procOn + 1; i < NUMOFPROCS; i++){
			if(uniprocs[i].status == 1){
				waittimes[i]++;
				waitTime++;
			}
		}
		
		// if the process went to zero time, terminate it!
		if(uniprocs[procOn].c == 0){
			uniprocs[procOn].status = 3;
			ftimes[procOn] = time + 1;
			procOn++;
		}
		// that was one clock cycle, dawg
		time++;
		
		// update status
		uniprocs[procOn].status = 2;
		
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
