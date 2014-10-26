#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "processStruct.h" // struct to handle processes
#include "myrand.h" // random number generator

// first thing - copy over procs into another array - DO NOT MESS WITH PROCS[]
void runfcfs(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs [])
{

	randInit();
	
	int finishtimes[NUMOFPROCS];
	int iotimes[NUMOFPROCS];
	int waittimes[NUMOFPROCS];
	int cputime = 0;
	int iocycles = 0;
	int completedProcs = 0;
	int time = 0;
	int procOn;
	int somerunflag = 0;
	
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
	}
	
	while(completedProcs < NUMOFPROCS){
	
		int iohappened = 0;
		
		if(verbose > 0){
			printverb(time, NUMOFPROCS, fcprocs);
		}
		
		for(procOn = 0; procOn < NUMOFPROCS; procOn++){
		
			// running
			if(fcprocs[procOn].status == 2){
				fcprocs[procOn].b--;
				fcprocs[procOn].c--;
				cputime++;
				if(fcprocs[procOn].c == 0){
					// terminate process
					completedProcs++;
					fcprocs[procOn].status = 4;
					finishtimes[procOn] = time;
					somerunflag = 0;
				}
				else if(fcprocs[procOn].b == 0){
					fcprocs[procOn].io = getBurst(sprocs[procOn].io, verbose);
					fcprocs[procOn].status = 3;
					somerunflag = 0;
				}
			}
			
			// io
			else if(fcprocs[procOn].status == 3){
				fcprocs[procOn].io--;
				iotimes[procOn]++;
				iohappened = 1;
				if(fcprocs[procOn].io == 0){
					fcprocs[procOn].b = getBurst(sprocs[procOn].b, verbose);
					fcprocs[procOn].status = 1;
				}
			}
			
			else if(fcprocs[procOn].status == 1){
				waittimes[procOn]++;
			}
			
			// check to initialize
			else if(fcprocs[procOn].status == 0){
				if(fcprocs[procOn].a == time){
					int n;
					int runningflag = 0;
					for(n = 0; n < NUMOFPROCS; n++){
						if(fcprocs[n].status == 2){
							runningflag = 1;
						}
					}
					if(runningflag == 0){ // start this one!
						fcprocs[procOn].status = 2;
					}
					else{ // already something running
						fcprocs[procOn].status = 1;
					}
					fcprocs[procOn].b = getBurst(sprocs[procOn].b, verbose);
				}
			}
		
		}
		
		if(completedProcs < NUMOFPROCS && !somerunflag){
			int curproc = procOn + 1;
			while(curproc < NUMOFPROCS){
				if(fcprocs[curproc].status == 1){
					fcprocs[curproc].status = 2;
					break;
				}
			}
			if(curproc == NUMOFPROCS){
				curproc = 0;
				while(curproc <= procOn){
					if(fcprocs[curproc].status == 1){
						fcprocs[curproc].status = 2;
						break;
					}
				}
			}
		}
		
		if(iohappened){
			iocycles++;
		}
		
		if(completedProcs < NUMOFPROCS){ // don't increment time if all process have completed
			time++;
		}
		
	}
}