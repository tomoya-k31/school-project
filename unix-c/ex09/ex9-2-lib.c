#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
  FILE *fp1 = fopen("temp2.txt","w+");
  if(!fp1){perror("fopen");return 1;}
  int c = 'a';
  int n=0;
  while(n++<10240){
    fputc(c,fp1);
  }
  fclose(fp1);

  FILE *fp2 = fopen("temp2.txt","r");
  if(!fp2){perror("fopen");return 1;}
  while(!feof(fp2)){
    c = fgetc(fp2);
  }

  fclose(fp2);

  return 0;
}
