#include <stdio.h>
#include <stdlib.h>
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

void fprintverb(int NUMOFPROCS, struct process procs[])
{
	int i;
	for(i = 0; i < NUMOFPROCS; i++){
		printf("%i %i %i %i\n", procs[i].a, procs[i].b, procs[i].c, procs[i].io);
	}
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
	int nextcpu = 0;
	int nextio = 0;
	for(i = 0; i < NUMOFPROCS; i++){
		cputimes[i] = 0; // initialize all to zero
		iotimes[i] = 0;
		readycpu[i] = 0;
		readyio[i] = 0;
	}
	
	while(completedprocs < NUMOFPROCS){
		
		// run cpu for running process
		if(readycpu[nextcpu] > 0){
			readycpu[nextcpu]--;
			cputime++;
			cputimes[nextcpu]++;
			fcprocs[nextcpu].c--; // decrement actual necessary cpu time
			if(fcprocs[nextcpu].c == 0){
				completedprocs++;
				// terminate it
				// shift nextcpu to next place
				nextcpu = incr(nextcpu);
				// increment lower bound of arlims to keep nextcpu & nextio in valid range
				arlims[0]++;
			}
			else if(readycpu[nextcpu] == 0){
				readyio[nextcpu] = fcprocs[nextcpu].io;
				// shift nextcpu to next place
				nextcpu = incr(nextcpu);
			}
		}
		
		// io for next available process
		if(readyio[nextio] > 0){
			readyio[nextio]--;
			iotime++;
			iotimes[nextio]++;
			if(readyio[nextio] == 0){
				readycpu[nextio] = fcprocs[nextio].b; // cpu is ready to go again
				// shift nextio to next place
				nextio = incr(nextio);
			}
		}
		
		for(i = 0; i < NUMOFPROCS; i++){
			if(time == fcprocs[i].a){
				readycpu[i] = getBurst(fcprocs[i].b);
				arlims[1]++; // allow nextcpu & nextio to advance to newly added process
			}
		}
		
		printf("time: %i\n", time);
		printf("readycpu:");
		for(i = 0; i < NUMOFPROCS; i++){
			printf(" %i", readycpu[i]);
		}
		printf("\nreadyio:");
		for(i = 0; i < NUMOFPROCS; i++){
			printf(" %i", readyio[i]);
		}printf("\n");
				
		time++;
	
	}
	
	fprintverb(NUMOFPROCS, sprocs);
	printf("%i\n", time);

}