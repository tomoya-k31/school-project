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
  if(!fin){perror("fopen");return 1;}
  char s[16];
  int idx = 1,flag = 1;
  while (fgets(s, sizeof(s), fin) != NULL) {
    if(flag){
      printf("%6d\t",idx++);
      flag = 0;
    }
    char *ret;
    fputs(s, stdout);
    if((ret = strchr(s,'\n')))
      flag = 1;
  }
  return 0;
}
