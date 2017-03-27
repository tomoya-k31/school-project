#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
  int fd0 = open("temp.txt",O_RDWR|O_CREAT|O_TRUNC,0644);
  if(fd0==-1){perror("open to read");return 1;}

  size_t size = 1;
  char a = 'a';
  int n = 0;
  while(n++!=10240){
    write(fd0,&a,size);
  }
  if (close(fd0) == -1) { perror("close"); return 1; }

  fd0 = open("temp.txt",O_RDONLY);
  if(fd0==-1){perror("open to read");return 1;}
  while(n){
    n = read(fd0,&a,size);
    if(n == -1) return 1;
  }

  if (close(fd0) == -1) { perror("close"); return 1; }

  return 0;
}
