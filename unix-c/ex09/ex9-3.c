#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
  char a = 'a';
  int fd = open("text.txt",O_WRONLY|O_CREAT|O_TRUNC,0664);
  if(fd == -1) {perror("write open");return 1;}
  lseek(fd,1000,SEEK_CUR);
  write(fd,&a,1);
  if(close(fd) == -1)return 1;

  fd = open("text.txt",O_RDONLY);
  if(fd == -1) {perror("read open");return 1;}
  while(read(fd,&a,1) > 0){
    switch(a){
      case 0:
        printf("0");
        break;
      case 'a':
        printf("a");
        break;
      default:
        printf("?");
    }
  }
  printf("\n");
  if(close(fd) == -1)return 1;

  return 0;
}
