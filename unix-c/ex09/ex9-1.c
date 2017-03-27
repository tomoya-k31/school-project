#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(stderr, "usage:%s source destination\n", argv[0]);
    return 1;
  } 

  int fd0 = open(argv[1],O_RDONLY);
  if(fd0==-1){perror("open to read");return 1;}
  int fd1 = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644);
  if(fd1==-1){perror("open to write");return 1;}

  size_t size = 10;
  char buf[size];
  while(1){
    int n = read(fd0,buf,size);
    if(n==0)break;
    if(n == -1) return 1;
    write(fd1,buf,n);

  }
  if (close(fd0) == -1) { perror("close"); return 1; }
  if (close(fd1) == -1) { perror("close"); return 1; }

  return 0;
}
