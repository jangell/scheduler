#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "myrand.h"

char fileName[] = "randnums.txt";
FILE * fp;
int maxRand;
int minRand;

void randInit()
{
  fp = fopen(fileName, "r");
}

int closeRand()
{
  return fclose(fp);
}

int getRand()
{
	int i = 0;
  if(fp == 0){
    randInit();
  }
  fscanf(fp, "%i", &i);
  if(i == 0){
    printf("ERROR - END OF FILE");
  }
  return i; // returns int between ~1 and MAX_INT
}