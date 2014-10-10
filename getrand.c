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

float getRand()
{
	float f = 0;
  if(fp == 0){
    randInit();
  }
  fscanf(fp, "%f", &f);
  if(f == 0){
    printf("ERROR - END OF FILE");
  }
  return f/INT_MAX; // returns a floating-point value between 0 and 1 
}