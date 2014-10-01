#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argv, char * argc[])
{
  
  int verbose = 0;
  int show_rand = 0;
  FILE * fp;
  
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
  
  

}
