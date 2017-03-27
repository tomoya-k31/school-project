#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if(argc != 2){
    fprintf(stderr, "usage:%s source destination\n", argv[0]);
    return 1;
  }
  FILE *fin = fopen(argv[1],"r");
  if(!fin) { perror("fopen");return 1;}
  size_t sz = 16;
  char *str = malloc(sz);
  //if(!str) { perror("malloc");return 1;}
  if (str == NULL) sz = 0;
  int idx = 1;
  while (getline(&str, &sz, fin) != -1){
    printf("%6d\t%s",idx++,str);
  }
  return 0;
}
