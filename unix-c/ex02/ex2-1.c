#include <stdio.h>
#include <string.h>
#define SZ 10

char* find(char d[][SZ], int n, char* s);

int main(int argc, char *argv[])
{
  char d[5][SZ] = {"abc", "xyz", "nnn", "ii", "jj"};
  char *p = find(d, 5, "ii");
  if (p) printf("found\n");
}

char* find(char d[][SZ], int n, char* s){
  for(int i=0;i<n;i++){
    if(strncmp(d[i],s,SZ)==0){
      return d[i];
    }
  }
  return NULL;
}
