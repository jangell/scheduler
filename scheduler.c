#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "myrand.h"

struct process
{
  int a;
  int b;
  int c;
  int io;
};

int getNextInt(FILE * fp)
{
  char cur;
  while(! isdigit(cur = fgetc(fp))){}
  return (cur - '0');
}

int main(int argv, char * argc[])
{
  
  int verbose = 0;
  int show_rand = 0;
  FILE * fp = 0;
  
  int numArgs = argv;
  
  if(numArgs == 2){
    fp = fopen(argc[1], "r");
  }

  else if(numArgs == 3){
    if(strncmp(argc[1], "--verbose", 10) == 0){
      verbose = 1;
      printf("running in verbose mode\n");
    }
    else if(strncmp(argc[1], "--show-random", 13) == 0){
      show_rand = 1;
      printf("running in show-random mode\n");
    }
    else{
      printf("Illegal argument given: %s\n", argc[1]);
      printf("Proceeding in non-verbose mode\n");
    }
    fp = fopen(argc[2], "r");
  }

  if(fp == NULL){
    printf("FATAL ERROR: invalid file given\n");
    exit(1);
  }

  char nextChar = ' ';
  int NUMOFPROCS = 0;
  while(NUMOFPROCS == 0){
    nextChar = fgetc(fp);
    if(isdigit(nextChar)){
      NUMOFPROCS = nextChar - '0'; // subtracts the ascii value for 0 from int value
    }
  }

  int i = 0;
  struct process procs[NUMOFPROCS];
  while(i < NUMOFPROCS){
    procs[i].a = getNextInt(fp);
    procs[i].b = getNextInt(fp);
    procs[i].c = getNextInt(fp);
    procs[i].io = getNextInt(fp);
    i++;
  }

  int j = 0;
  while(j < NUMOFPROCS){
    printf("%i %i %i %i\n", procs[j].a, procs[j].b, procs[j].c, procs[j].io);
    j++;
  }
  
  fclose(fp);
  
}
