#include <stdio.h>
#include <stdlib.h>
#include "myrand.h"

char fileName[] = "randnums.txt";
FILE * fp;

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
  if(fp == 0){
    randInit();
  }
  int i = 0;
  fscanf(fp, "%d", &i);
  if(i == 0){
    printf("ERROR - END OF FILE");
  }
  return i; 
}
