#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <limits.h>

char** get_strings(int* nump);

int main(int argc, char *argv[])
{
  int  num;
  char **str = get_strings(&num);
  for (int i = 0; i < num; i++)
    printf("%s\n", str[i]);
  printf("\n");
}

char** get_strings(int* nump){
  char **str = malloc(sizeof(char*));
  if (!str) { perror("malloc"); return 0; }
  char *tmp;
  int size=1;
  (*nump)=0;
  while(scanf("%ms",&tmp) > 0){
    if(*nump>=size){
      size += (*nump);
      str = realloc(str,sizeof(char*)*size);
      if (!str) { perror("realloc"); return 0; }
    }
    str[(*nump)++] = tmp;
  }
  free(tmp);
  return str;
}
