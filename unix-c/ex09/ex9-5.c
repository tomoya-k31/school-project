#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
  if(argc<2){
    perror("argv");
    return 1;
  }
  int fd = open(argv[1],O_RDONLY);
  if(fd == -1) {perror("write open");return 1;}
  off_t size = lseek(fd,0,SEEK_END);

  printf("the file size of %s is %ld bytes\n",argv[1],size);

  if(close(fd) == -1)return 1;

  return 0;
}
