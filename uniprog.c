#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "myrand.h"
#include "processStruct.h" // {a, b, c, io, status}
#include "uniprog.h" // uni header
#include "printing.h"

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

void runUni(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs []){

	randInit();
	
	// struct of processes on which the scheduler is based
	struct process uniprocs[NUMOFPROCS];
	
	// copy sprocs over into another (mutable) array
	// for the extent of the uniprocessor, use uniprocs, NOT sprocs
	int i;
	for(i = 0; i < NUMOFPROCS; i++){
		uniprocs[i] = sprocs[i];
	}
	
	int procStarts[NUMOFPROCS]; // array to keep track of sorted processes
	
	int ftimes[NUMOFPROCS]; // finish times
	int iotimes[NUMOFPROCS]; // io times
	int waittimes[NUMOFPROCS]; // waiting times
	int time = 0; // running time
	int cpuTime = 0; // total cpu time used thus far
	int ioTime = 0; // total io time used thus far
	int waitTime = 0; // total waiting time of process
	
	for(i = 0; i < NUMOFPROCS; i++){
		procStarts[i] = 0; // initialize all to "unused"
		iotimes[i] = 0; // io times start at 0
		waittimes[i] = 0; // wait times start at 0
	}
	
	// preprinting:
	
	printf("The original input was: %i", NUMOFPROCS);
	printprocs(NUMOFPROCS, procs);
	
	printf("The (sorted) input is:  %i", NUMOFPROCS);
	printprocs(NUMOFPROCS, sprocs);
	
	if(verbose > 0){
		printf("\nThis detailed printout gives the state and remaining burst for each process\n\n");
	}
	
	int procOn = 0; // index of current running procedure
	int nextBurst = 0;
	int task = 1; // flag: 0 = cpu burst, 1 = io burst
	// inverts BEFORE getting burst size; therefore, cpu comes first if 1 here
	int cputogo = 0; // cpu time to go in current process ("b")
	
	
	while(procOn < NUMOFPROCS){
		
		// let's do this. butter side down
		if(uniprocs[procOn].a > time){
			cputogo = uniprocs[procOn].c; // processing time left on the current task
			
		}
		
		// print start statuses here
		if(verbose > 0){
			printverb(NUMOFPROCS, time, task, nextBurst, uniprocs);
		}
		
		// increment wait times of all waiting processes (index > procOn)
		int i;
		for(i = procOn + 1; i < NUMOFPROCS; i++){
			waitTime++;
			waittimes[i]++;
		}
		
		// run current process, either cpu or io
		if(cputogo > 0){
			// get new burst if burst is zero
			if(nextBurst == 0){
				task = (task + 1)%2; // returns (2%2) = 0 if 1 and (1%2) = 1 if 0
				if(task == 0){
					nextBurst = getBurst(uniprocs[procOn].b, verbose);
				}
				else if(task == 1){
					nextBurst = getBurst(uniprocs[procOn].io, verbose);
				}
			}
			if(nextBurst > 0){
				nextBurst--;
				time++;
				if(task == 0){
					cputogo--;
					cpuTime++;
				}
				else if(task == 1){
					ioTime++;
					iotimes[procOn]++;
				}
			}
			if(cputogo == 0){
				ftimes[procOn] = time;
				procOn++;
			}
		}
		
		// aaaand that was one clock cycle!
		time++;
		
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
