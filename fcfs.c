#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "processStruct.h" // struct to handle processes
#include "myrand.h" // random number generator

int arlims[2] = {0, 0}; // used to keep nextcpu in valid range

int incr(int ind){
	ind++;
	if(ind == arlims[1]){
		ind = arlims[0];
	}
	return ind;
}

void fprintverb(int NUMOFPROCS, int time, int readycpu [], int readyio [], int status [])
{
	int i;
	char statString[20];
	int statVal;
	printf("Before cycle %6i:", time);
	for(i = 0; i < NUMOFPROCS; i++){
		if(status[i] == 0){
			strcpy(statString, "unstarted");
			statVal = 0;
		}
		else if(status[i] == 1){
			strcpy(statString, "ready");
			statVal = 0;
		}
		else if(status[i] == 2){
			strcpy(statString, "running");
			statVal = readycpu[i];
		}
		else if(status[i] == 3){
			strcpy(statString, "blocked");
			statVal = readyio[i];
		}
		else if(status[i] == 4){
			strcpy(statString, "terminated");
			statVal = 0;
		}
		else{
			strcpy(statString, "ERROR - BAD STATUS");
			statVal = -1;
		}
		printf("%14s:%6i", statString, statVal);
		
	}
	printf("\n");
}

// first thing - copy over procs into another array - DO NOT MESS WITH PROCS[]
void runfcfs(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs [])
{

	randInit();
	
	struct process fcprocs[NUMOFPROCS];
	int i;
	for(i = 0; i < NUMOFPROCS; i++){
		fcprocs[i] = sprocs[i];
	}
	
	// why is this already real scheduling? who cares! let's do this! butter side down!
	int time = 0;
	int cputime = 0;
	int cputimes[NUMOFPROCS];
	int iotime = 0;
	int iotimes[NUMOFPROCS];
	int completedprocs = 0;
	
	int readycpu[NUMOFPROCS];
	int readyio[NUMOFPROCS];
	int status[NUMOFPROCS];
	int nextcpu = 0;
	int nextio = 0;
	for(i = 0; i < NUMOFPROCS; i++){
		cputimes[i] = 0; // initialize all to zero
		iotimes[i] = 0;
		readycpu[i] = 0;
		readyio[i] = 0;
		status[i] = 0; // not the same as uniprog
		// 0=unstarted; 1=ready; 2=running; 3=blocked; 4=terminated
	}
	
	int tempio; // used to postpone updating the IO to avoid simultaneous cpu & io
	int tempind; // used as index for above thing
	
	while(completedprocs < NUMOFPROCS){
		
		fprintverb(NUMOFPROCS, time, readycpu, readyio, status);
		
		// scan for newly added processes
		for(i = 0; i < NUMOFPROCS; i++){
			if(time == fcprocs[i].a){
				status[i] = 1; // move from unstarted to ready
				readycpu[i] = getBurst(fcprocs[i].b);
				arlims[1]++; // allow nextcpu & nextio to advance to newly added process
			}
		}
		
		// run cpu for running process
		tempio = 0;
		if(readycpu[nextcpu] > 0){
			readycpu[nextcpu]--;
			cputime++;
			cputimes[nextcpu]++;
			fcprocs[nextcpu].c--; // decrement actual necessary cpu time
			if(fcprocs[nextcpu].c == 0){
				// terminate it
				completedprocs++;
				status[nextcpu] = 4;
				// shift nextcpu to next place
				nextcpu = incr(nextcpu);
				// increment lower bound of arlims to keep nextcpu & nextio in valid range
				arlims[0]++;
			}
			else if(readycpu[nextcpu] == 0){
				tempio = getBurst(fcprocs[nextcpu].io, verbose);
				tempind = nextcpu;
				status[nextcpu] = 3;
				// shift nextcpu to next place
				nextcpu = incr(nextcpu);
			}
			status[nextcpu] = 2; // currently running thing
		}
		
		// io for next available process
		if(readyio[nextio] > 0){
			readyio[nextio]--;
			iotime++;
			iotimes[nextio]++;
			if(readyio[nextio] == 0){
				readycpu[nextio] = getBurst(fcprocs[nextio].b, verbose); // cpu is ready to go again
				status[nextio] = 1;
				// shift nextio to next place
				nextio = incr(nextio);
			}
			if(readyio[nextio] > 0){ // should not set status to io if io is 0
				status[nextio] = 3;
			}
		}
		
		// update io here to avoid allowing processes to double up w/ cpu & io in one cycle
		if(tempio > 0){
			readyio[tempind] = tempio;
		}
		
		if(completedprocs < NUMOFPROCS){
			time++;
		}
	
	}

	printf("%i\n", time);

}