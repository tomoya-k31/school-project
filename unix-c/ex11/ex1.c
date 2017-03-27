#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>


int main(int argc, char *argv[])
{
  char *buf;
  while(buf=getcwd(NULL,0)){
    
    printf("%s\n",buf);

    if(!strcmp(buf,"/")){
      break;
    }

    if(chdir("..") == -1){
      perror("chdir");
      return 1;
    }
  }

  free(buf);

  return 0;
}
