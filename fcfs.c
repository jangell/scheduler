#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#include "processStruct.h" // struct to handle processes
#include "myrand.h" // random number generator


void runfcfs(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs [])
{

	randInit();
	
	int finishtimes[NUMOFPROCS];
	int iotimes[NUMOFPROCS];
	int totalio = 0;
	int ioflag = 0;
	int waittimes[NUMOFPROCS];
	int cputime = 0;
	int completedProcs = 0;
	int time = 0;
	int procOn;
	int somethingisrunning = 0;
	int lastran[NUMOFPROCS];
	
	struct process fcprocs[NUMOFPROCS];
	int i;
	for(i = 0; i < NUMOFPROCS; i++){
		fcprocs[i] = sprocs[i];
		fcprocs[i].status = 0; // 0=unstarted; 1=ready; 2=running; 3=blocked; 4=terminated
		fcprocs[i].b = 0; // no cpu burst to start
		fcprocs[i].io = 0; // no io burst to start
		
		finishtimes[i] = 0;
		iotimes[i] = 0;
		waittimes[i] = 0;
		lastran[i] = fcprocs[i].a;
	}
	
	printf("The original input was: ");
	printprocs(NUMOFPROCS, procs);
	printf("The (sorted) input is:  ");
	printprocs(NUMOFPROCS, sprocs);
	printf("\nThis detailed printout gives the state and remaining burst for each process\n\n");
	
	while(completedProcs < NUMOFPROCS){
	
		ioflag = 0;
		
		if(verbose > 0){
			printverb(time, NUMOFPROCS, fcprocs);
		}
		
		for(procOn = 0; procOn < NUMOFPROCS; procOn++){
			
			if(fcprocs[procOn].status == 2){
				// running
				fcprocs[procOn].b--;
				fcprocs[procOn].c--;
				cputime++;
				if(fcprocs[procOn].c == 0){
					// terminate it
					fcprocs[procOn].status = 4;
					finishtimes[procOn] = time;
					completedProcs++;
					somethingisrunning = 0;
				}
				else if(fcprocs[procOn].b == 0){
					// update status and last-ran-time
					fcprocs[procOn].status = 3;
					fcprocs[procOn].io = getBurst(sprocs[procOn].io, verbose);
					somethingisrunning = 0;
				}
			}
			else if(fcprocs[procOn].status == 3){
				fcprocs[procOn].io--;
				iotimes[procOn]++;
				ioflag = 1;
				if(fcprocs[procOn].io == 0){
					lastran[procOn] = time;
					fcprocs[procOn].status = 1;
				}
			}
			else if(fcprocs[procOn].status == 1){
				waittimes[procOn]++;
			}
			else if(fcprocs[procOn].status == 0 && fcprocs[procOn].a == time){
				fcprocs[procOn].status = 1;
			}
			
		}
		
		if(!somethingisrunning){
			int torunindex = -1;
			int torunlast = INT_MAX;
			for(i = 0; i < NUMOFPROCS; i++){
				if(fcprocs[i].status == 1 && lastran[i] < torunlast){
					torunindex = i;
					torunlast = lastran[i];
				}
			}
			if(torunindex == -1){} // just keep going; increment time and try again next cycle
			else{
				fcprocs[torunindex].status = 2;
				somethingisrunning = 1;
				fcprocs[torunindex].b = getBurst(sprocs[torunindex].b, verbose);
			}
		}
		
		if(ioflag){
			totalio++;
		}
		
		if(completedProcs < NUMOFPROCS){ // don't increment time if all process have completed
			time++;
		}
	}
	
	closeRand();
	
	// print summaries of each process
	
	printf("The scheduling algorithm used was First Come First Served\n\n");
	for(i = 0; i < NUMOFPROCS; i++){
		printf("Process %i:\n", i);
		printf("\t(A,B,C,IO) = (%i,%i,%i,%i)\n", sprocs[i].a, sprocs[i].b, sprocs[i].c, sprocs[i].io);
		printf("\tFinishing time: %i\n", finishtimes[i]);
		printf("\tTurnaround time: %i\n", (finishtimes[i] - sprocs[i].a));
		printf("\tI/O time: %i\n", iotimes[i]);
		printf("\tWaiting time: %i\n\n", waittimes[i]);
	}
	
	// generate values for summary below

	int totalTurn = 0;
	int totalWait = 0;
	for(i = 0; i < NUMOFPROCS; i++){
		totalTurn += (finishtimes[i] - sprocs[i].a); // finish - start = turn
		totalWait += waittimes[i];
	}
	float cpuuse = ((float) cputime) / time;
	float iouse = ((float) totalio) / time;
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