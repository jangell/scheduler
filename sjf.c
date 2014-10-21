#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#include "myrand.h" // random number generator, getBurst()
#include "processStruct.h" // struct to hold processes

// node struct for io linked list
struct node
{
	int val;
	struct node * next;
};

void sprintverb(int NUMOFPROCS, int time, int sjob, struct process procs [], struct node * head)
{
	int i;
	printf("Before cycle");
	printf("%6i:", time);
	for(i = 0; i < NUMOFPROCS; i++){
		int stat = procs[i].status;
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
			strcpy(statString, "running");
			statVal = procs[sjob].b;
		}
		else if(stat == 3){
			strcpy(statString, "blocked");
			statVal = procs[(*(*head).next).val].io;
		}
		else if(stat == 4){
			strcpy(statString, "terminated");
			statVal = 0;
		}
		else{
			strcpy(statString, "ERROR: stat > 3");
			statVal = -1;
		}
		printf("\t%12s%3i", statString, statVal);
	}
	printf(".\n");
}


void runsjf(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs [])
{
	
	randInit();
	
	struct process sjfprocs[NUMOFPROCS];
	
	int iotimes[NUMOFPROCS];
	int waittimes[NUMOFPROCS];
	int cputimes[NUMOFPROCS];
	struct node iobase;
	iobase.val = 0;
	iobase.next = NULL;
	struct node * iohead = NULL;
	
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
		cputimes[i] = 0;
	}
	
	int completedProcs = 0;
	int time = 0;
	int sjob = 0;
	
	while(completedProcs < NUMOFPROCS){
	
		sprintverb(NUMOFPROCS, time, sjob, sjfprocs, iohead);
	
		// look for processes to start (by time)
		for(i = 0; i < NUMOFPROCS; i++){
			if(sjfprocs[i].a == time){
				// initialize procedure with status and burst time
				sjfprocs[i].status = 1;
				sjfprocs[i].b = getBurst(sprocs[i].b);
			}
		}
		
		// find new burst if next burst is zero
		if(sjfprocs[sjob].b == 0){
			int sjob = INT_MAX;
			// find index of shortest remaining job
			for(i = 0; i < NUMOFPROCS; i++){
				if(sjfprocs[i].status == 1 && sjfprocs[i].c < sjob){
					sjob = i;
				}
			}
			// only assign next burst if a burst is found
			if(sjob == INT_MAX){
				sjob = -1; // flag for no valid cpu burst to run
			}
		}
		// sjob should now be the index of the next process to run
		// OR -1, in which case nothing should run
		
		// run process
		if(sjob > -1){
			sjfprocs[sjob].status = 2;
			sjfprocs[sjob].b--; // decrement burst time
			sjfprocs[sjob].c--; // decrement total cpu time remaining
			// check if we hit the end of the process
			if(sjfprocs[sjob].c == 0){
				// terminate process
				sjfprocs[sjob].status = 4;
				completedProcs++;
			}
			// check if we hit the end of the burst (not including the end of the whole process)
			else if(sjfprocs[sjob].b == 0){
				struct node nextio;
				nextio.val = getBurst(sprocs[sjob].io, verbose); // add new io burst to l-list
				nextio.next = NULL;
				// add nextio to the end of the io list
				struct node * addio = iohead;
				printf("a\n");
				while((*addio).next != NULL){
					addio = (*addio).next;
				}
				(*addio).next = &nextio;
				sjfprocs[sjob].status = 3;
			}
		}
		
		// check for io; if it exists, do it
		if((*iohead).next != NULL){
			struct node curio = (*(*iohead).next);
			int ioindex = curio.val;
			sjfprocs[ioindex].io--;
			iotimes[ioindex]++;
			// check if io burst is finished
			if(sjfprocs[ioindex].io == 0){
				sjfprocs[ioindex].b = getBurst(sprocs[ioindex].b, verbose);
				sjfprocs[ioindex].status = 1;
				// update linked list
				(*iohead).next = curio.next;
			}
		}
		
		time++;
		
	}
	
	printf("time: %i\n", time);
	
}