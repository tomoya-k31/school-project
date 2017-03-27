#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void func(int sig)
{
  printf("caught SIGALRM (%d)\n", sig);
}

int main(int argc, char *argv[])
{
  signal(SIGALRM, func);
  signal(SIGINT, SIG_IGN);
  alarm(3);  /*3秒後にアラームシグナルがくる*/
  pause();
  signal(SIGINT, SIG_DFL);
  alarm(3);  /*3秒後にアラームシグナルがくる*/
  pause();

  return 0;
}
