#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#include "myrand.h" // random number generator, getBurst()
#include "processStruct.h" // struct to hold processes


void runsjf(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs [])
{
	
	randInit();
	
	struct process sjfprocs[NUMOFPROCS];
	
	int iotimes[NUMOFPROCS];
	int waittimes[NUMOFPROCS];
	int finishtimes[NUMOFPROCS];
	int cputime = 0;
	
	int i;
	for(i = 0; i < NUMOFPROCS; i++){
		// copy over sprocs to another array (use sprocs as reference later on)
		sjfprocs[i] = sprocs[i];
		sjfprocs[i].status = 0; // 0=unstarted; 1=ready; 2=running; 3=blocked; 4=terminated
		sjfprocs[i].io = 0; // start without io to do
		sjfprocs[i].b = 0; // start without burst to run
		// set initial values of iotimes, waittimes, and cputimes to 0
		iotimes[i] = 0;
		waittimes[i] = 0;
	}
	
	int completedProcs = 0;
	int somethingisrunning = 0;
	int time = 0;
	
	while(completedProcs < NUMOFPROCS){
		
		// check for verbosity and print if on
		if(verbose > 0){
			printverb(time, NUMOFPROCS, sjfprocs);
		}
		
		// check what the status of each process is and proceed accordingly
		int procOn;
		for(procOn = 0; procOn < NUMOFPROCS; procOn++){
		
			if(sjfprocs[procOn].status == 2){
				// run process
				sjfprocs[procOn].b--;
				sjfprocs[procOn].c--;
				cputime++;
				if(sjfprocs[procOn].c == 0){
					// terminate
					sjfprocs[procOn].status = 4;
					finishtimes[procOn] = time;
					completedProcs++;
					somethingisrunning = 0;
				}
				else if(sjfprocs[procOn].b == 0){
					sjfprocs[procOn].status = 3;
					sjfprocs[procOn].io = getBurst(sprocs[procOn].io, verbose);
					somethingisrunning = 0;
				}
			}
			else if(sjfprocs[procOn].status == 3){
				sjfprocs[procOn].io--;
				iotimes[procOn]++;
				if(sjfprocs[procOn].io == 0){
					sjfprocs[procOn].status = 1; // don't generate burst yet
				}
			}
			else if(sjfprocs[procOn].status == 0 && sjfprocs[procOn].a == time){
				sjfprocs[procOn].status = 1; // unstarted -> ready
			}
			else if(sjfprocs[procOn].status == 1){
				waittimes[procOn]++;
			}
			
		}
		
		if(somethingisrunning == 0){
			// check to find ready process with shortest job
			int torun = -1;
			int minc = INT_MAX;
			for(i = 0; i < NUMOFPROCS; i++){
				if(sjfprocs[i].status == 1 && sjfprocs[i].c < minc){
					torun = i;
					minc = sjfprocs[i].c;
				}
			}
			if(torun == -1){}// just continue; somethingisrunning stays at 0
			else{
				sjfprocs[torun].status = 2;
				sjfprocs[torun].b = getBurst(sprocs[torun].b, verbose);
				somethingisrunning = 1;
			}
		}
		
		if(completedProcs < NUMOFPROCS){
			time++;
		}
		
	}
	
	printf("The original output was: ");
	printprocs(NUMOFPROCS, procs);
	printf("The (sorted) input is: ");
	printprocs(NUMOFPROCS, sprocs);
	printf("\n\n");
	
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