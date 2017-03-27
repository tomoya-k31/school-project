#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
  if(argc < 2){
    printf("usage %s dir name\n",argv[0]);
    return 1;
  }

  DIR *dfd = opendir(argv[1]);
  if(!dfd){
    perror("opendir");
    return 1;
  }

  struct dirent *dp;
  while((dp = readdir(dfd))){
    printf("%ld %s\n",dp->d_ino,dp->d_name);
  }

  closedir(dfd);

  return 0;
}
