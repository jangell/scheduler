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

void runsjf(int verbose, int NUMOFPROCS, struct process procs [], struct process sprocs [])
{
	
	randInit();
	
	
	struct process sjfprocs[NUMOFPROCS];
	
	int readycpu[NUMOFPROCS];
	int iotimes[NUMOFPROCS];
	int waittimes[NUMOFPROCS];
	int cputimes[NUMOFPROCS];
	struct node ioHead;
	ioHead.val = 0; // should never actually run IO from head
	ioHead.next = NULL; // null pointer signals end of linked list
	
	int i;
	for(i = 0; i < NUMOFPROCS; i++){
		// copy over sprocs to another array
		sjfprocs[i] = sprocs[i];
		sjfprocs[i].status = 0; // 0=unstarted; 1=ready; 2=running; 3=blocked; 4=terminated
		// set initial values of readycpu, iotimes, waittimes, and cputimes to 0
		readycpu[i] = 0;
		iotimes[i] = 0;
		waittimes[i] = 0;
		cputimes[i] = 0;
	}
	
	int completedProcs = 0;
	int time = 0;
	int nextBurst = 0;
	
	while(completedProcs < NUMOFPROCS){
	
		// look for processes to start (by time)
		for(i = 0; i < NUMOFPROCS; i++){
			if(sjfprocs[i].a == time){
				// initialize procedure with status and burst time
				sjfprocs[i].status = 1;
				readycpu[i] = getBurst(sjfprocs[i].b);
			}
		}
		
		if(nextBurst == 0){
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
		
		
		
		time++;
		
	}
	
	printf("time: %i\n", time);
	
}