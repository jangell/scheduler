#ifndef _PROCESSSTRUCT_H_
#define _PROCESSSTRUCT_H_

// struct to organize each process by its a, b, c, & io #s
struct process
{
  int a;
  int b;
  int c;
  int io;
  char status; // 0=unstarted, 1=ready, 2=running/blocked (look at b & d), 3=terminated
};

#endif
