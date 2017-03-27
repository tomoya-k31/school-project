#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if(argc != 2){
    fprintf(stderr, "usage:%s source destination\n", argv[0]);
    return 1;
  } 

  FILE *fp = fopen(argv[1],"rb");
  if(!fp){perror("fopen");return 1;}

  fseek(fp,18,SEEK_SET);

  int x[2];
  fread(x,sizeof(int),2,fp);
  printf("width:%dpixel\n",x[0]);
  printf("height:%dpixel\n",x[1]);

  fclose(fp);

  return 0;
}
