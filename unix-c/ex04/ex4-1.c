#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <limits.h>

int main(int argc, char *argv[])
{
  int x;
  scanf("%d", &x);
  double *data = malloc(sizeof(double)*x);
  if(!data){printf("ERROR!\n");return -1;}
  for (int i=0;i<x;i++)
    scanf("%lf",&data[i]);
  for (int i=0;i<x;i++){
    if(i!=0) printf(", ");
    printf("%f",data[i]);
  }
  printf("\n");
}
