#include <stdio.h>
#include <stdlib.h>
#include "processStruct.h" // struct to handle processes
#include "myrand.h" // random number generator

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
	int readycpu[NUMOFPROCS];
	int readyio[NUMOFPROCS];
	int nextcpu = 0;
	int nextio = 0;
	for(i = 0; i < NUMOFPROCS; i++){
		readycpu[i] = 0;
		readyio[i] = 0;
	}
	
	while(1){
		
		if(readycpu[nextcpu] > 0){
			
		}
		
		for(i = 0; i < NUMOFPROCS; i++){
			if(time == fcprocs[i].a){
				readycpu[i] = fcprocs[i].b;
			}
		}
	
	}
	
	fprintverb(NUMOFPROCS, fcprocs);
	
}