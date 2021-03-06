#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "myrand.h"
#include "processStruct.h" // {a, b, c, io, status}

void runUni(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs []){

	randInit();
	
	// struct of processes on which the scheduler is based
	struct process uniprocs[NUMOFPROCS];
	
	int finishtimes[NUMOFPROCS];
	int iotimes[NUMOFPROCS];
	int waittimes[NUMOFPROCS];
	int cputime = 0;
	
	// copy sprocs over into another (mutable) array
	// use sprocs as reference for burst and io length of uniprocs
	int i;
	for(i = 0; i < NUMOFPROCS; i++){
		uniprocs[i] = sprocs[i];
		uniprocs[i].b = 0; // start without cpu burst
		uniprocs[i].io = 0; // start without io burst
		uniprocs[i].status = 0; // 0=unstarted; 1=ready; 2=running; 3=blocked; 4=terminated
		// initialize arrays to zero
		finishtimes[i] = 0;
		iotimes[i] = 0;
		waittimes[i] = 0;
	}
	
	// do that scheduling stuff
	int completedProcs = 0;
	int curproc = 0;
	int time = 0;
	int ioburstflag = 0; // flag to generate new io burst
	
	printf("The original input was: ");
	printprocs(NUMOFPROCS, procs);
	printf("The (sorted) input is:  ");
	printprocs(NUMOFPROCS, sprocs);
	printf("\nThis detailed printout gives the state and remaining burst for each process\n\n");
	
	// actual cycle loop
	while(completedProcs < NUMOFPROCS){
		
		if(verbose > 0){
			printverb(time, NUMOFPROCS, uniprocs); // print statuses
		}
		
		if(uniprocs[curproc].status == 2){
			// run cpu
			uniprocs[curproc].b--;
			uniprocs[curproc].c--;
			cputime++;
			if(uniprocs[curproc].c == 0){ // terminate process
				uniprocs[curproc].status = 4;
				finishtimes[curproc] = time;
				completedProcs++;
				curproc++;
				if(curproc < NUMOFPROCS){ // (not after last process terminates)
					uniprocs[curproc].status = 1; // burst generated in "generate new burst" block below
				}
			}
			else if(uniprocs[curproc].b == 0){ // terminate burst; generate io burst
				ioburstflag = 1;
			}
		}
		
		if(uniprocs[curproc].status == 3){
			iotimes[curproc]++;
			uniprocs[curproc].io--;
			if(uniprocs[curproc].io == 0){ // generate cpu burst
				uniprocs[curproc].status = 1; // burst generated below
			}
		}
		
		// generate io burst here to avoid running io and cpu for same process in same cycle
		if(ioburstflag){
			ioburstflag = 0; // reset flag
			uniprocs[curproc].io = getBurst(sprocs[curproc].io, verbose);
			uniprocs[curproc].status = 3;
		}
		
		// initialize new processes by time at the end of the cycle
		for(i = 0; i < NUMOFPROCS; i++){
			if(uniprocs[i].a == time){
				// initialize process
				uniprocs[i].status = 1;
			}
		}
		
		// generate new burst if necessary
		if(uniprocs[curproc].status == 1){
			uniprocs[curproc].b = getBurst(sprocs[curproc].b, verbose);
			uniprocs[curproc].status = 2; // running next cycle
		}
		
		// update wait times (cannot be waiting process before curproc
		for(i = curproc; i < NUMOFPROCS; i++){
			if(uniprocs[i].status == 1){
				waittimes[i]++;
			}
		}
		
		if(completedProcs < NUMOFPROCS){ // don't increment time if last process finished
			time++;
		}
		
	}
	
	closeRand();
	
	// print summaries of each process
	printf("\n");
	for(i = 0; i < NUMOFPROCS; i++){
		printf("Process %i:\n", i);
		printf("\t(A,B,C,IO) = (%i,%i,%i,%i)\n", sprocs[i].a, sprocs[i].b, sprocs[i].c, sprocs[i].io);
		printf("\tFinishing time: %i\n", finishtimes[i]);
		printf("\tTurnaround time: %i\n", (finishtimes[i] - sprocs[i].a));
		printf("\tI/O time: %i\n", iotimes[i]);
		printf("\tWaiting time: %i\n\n", waittimes[i]);
	}
	
	// generate values for summary below

	int totalIO = 0;
	int totalTurn = 0;
	int totalWait = 0;
	for(i = 0; i < NUMOFPROCS; i++){
		totalIO += iotimes[i];
		totalTurn += (finishtimes[i] - sprocs[i].a); // finish - start = turn
		totalWait += waittimes[i];
	}
	float cpuuse = ((float) cputime) / time;
	float iouse = ((float) totalIO) / time;
	float through = 100. * ((float) NUMOFPROCS) / time;
	float avgturn = ((float) totalTurn) / NUMOFPROCS;
	float avgwait = ((float) totalWait) / NUMOFPROCS;
	
	// print overall summary
	
	printf("Summary Data:\n");
	printf("\tFinishing time: %i\n", time);
	printf("\tCPU Utilization: %6f\n", cpuuse);
	printf("\tI/O Utilization: %6f\n", iouse);
	printf("\tThroughput: %6f processes per hundred cycles\n", through);
	printf("\tAverage turnaround time: %6f\n", avgturn);
	printf("\tAverage waiting time: %6f\n", avgwait);
	
}
