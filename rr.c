#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#include "processStruct.h"
#include "myrand.h"

// round robin scheduler; quanta of two
void runrr(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs [])
{
	
	randInit();
	
	int finishtimes[NUMOFPROCS];
	int iotimes[NUMOFPROCS];
	int waittimes[NUMOFPROCS];
	int lastran[NUMOFPROCS];
	int bvals[NUMOFPROCS];
	int cputime = 0;
	int completedProcs = 0;
	int time = 0;
	int procOn;
	int somethingisrunning = 0;
	
	struct process rrprocs[NUMOFPROCS];
	int i;
	for(i = 0; i < NUMOFPROCS; i++){
		rrprocs[i] = sprocs[i];
		rrprocs[i].status = 0; // 0=unstarted; 1=ready; 2=running; 3=blocked; 4=terminated
		rrprocs[i].b = 0; // no cpu burst to start
		rrprocs[i].io = 0; // no io burst to start
		
		finishtimes[i] = 0;
		iotimes[i] = 0;
		waittimes[i] = 0;
		lastran[i] = 0;
		bvals[i] = 0;
	}
	
	printf("The original input was: ");
	printprocs(NUMOFPROCS, procs);
	printf("The (sorted) input is: ");
	printprocs(NUMOFPROCS, sprocs);
	printf("\nThis detailed printout gives the state and remaining burst for each process\n\n");
	
	while(completedProcs < NUMOFPROCS){
		
		if(verbose > 0){
			printverb(time, NUMOFPROCS, rrprocs);
		}
		
		for(procOn = 0; procOn < NUMOFPROCS; procOn++){
		
			if(rrprocs[procOn].status == 2){
				cputime++;
				rrprocs[procOn].b--;
				rrprocs[procOn].c--;
				bvals[procOn]--;
				if(rrprocs[procOn].c == 0){
					// terminate process
					rrprocs[procOn].status = 4;
					completedProcs++;
					finishtimes[procOn] = time;
					somethingisrunning = 0;
				}
				else if(bvals[procOn] == 0){
					rrprocs[procOn].status = 3;
					rrprocs[procOn].io = getBurst(sprocs[procOn].io, verbose);
					somethingisrunning = 0;
				}
				else if(rrprocs[procOn].b == 0){
					rrprocs[procOn].status = 1;
					lastran[procOn] = time;
					somethingisrunning = 0;
				}
			}
			else if(rrprocs[procOn].status == 3){
				rrprocs[procOn].io--;
				iotimes[procOn]++;
				if(rrprocs[procOn].io == 0){
					rrprocs[procOn].status = 1;
					lastran[procOn] = time;
				}
			}
			else if(rrprocs[procOn].status == 1){
				waittimes[procOn]++;
			}
			else if(rrprocs[procOn].status == 0 && rrprocs[procOn].a == time){
				// initialize process
				rrprocs[procOn].status = 1;
				lastran[procOn] = time;
			}
			
		}
		
		if(!somethingisrunning){
			int torunindex = -1;
			int torunlast = INT_MAX;
			for(i = 0; i < NUMOFPROCS; i++){
				if(rrprocs[i].status == 1 && lastran[i] < torunlast){
					torunindex = i;
					torunlast = lastran[i];
				}
			}
			if(torunindex == -1){} // do nothing; wait until next cycle
			else{
				rrprocs[torunindex].status = 2;
				if(bvals[torunindex] == 0){
					bvals[torunindex] = getBurst(sprocs[torunindex].b, verbose);
				}
				rrprocs[torunindex].b = 2; // quantum = 2
				if(bvals[torunindex] == 1){
					rrprocs[torunindex].b = 1;
				}
				somethingisrunning = 1;
			}
		}
		
		if(completedProcs < NUMOFPROCS){
			time++;
		}
		
	}
	
	closeRand();
	
	// print summaries of each process
	
	for(i = 0; i < NUMOFPROCS; i++){
		printf("\n");
		printf("Process %i:\n", i);
		printf("\t(A,B,C,IO) = (%i,%i,%i,%i)\n", sprocs[i].a, sprocs[i].b, sprocs[i].c, sprocs[i].io);
		printf("\tFinishing time: %i\n", finishtimes[i]);
		printf("\tI/O time: %i\n", iotimes[i]);
		printf("\tWaiting time: %i\n", waittimes[i]);
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
	
	printf("\n");
	printf("Summary Data:\n");
	printf("\tFinishing time: %i\n", time);
	printf("\tCPU Utilization: %6f\n", cpuuse);
	printf("\tI/O Utilization: %6f\n", iouse);
	printf("\tThroughput: %6f processes per hundred cycles\n", through);
	printf("\tAverage turnaround time: %6f\n", avgturn);
	printf("\tAverage waiting time: %6f\n", avgwait);
	
}