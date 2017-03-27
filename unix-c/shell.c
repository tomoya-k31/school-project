/*
 *
 * main.c - created by us142059
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


char* gettoken(char **ptr) {
  char *tmp = *ptr; /* tmp[idx] の形で使用する */
  int idx = 0;

  if(tmp==NULL || tmp[idx]=='\0')
    return NULL;

  /* 先頭のスペースを読み飛ばす */
  while(tmp[idx]==' ')idx++;
  int st=idx;
  /* 単語の終わりを見つける */
  while(tmp[idx]!='\0' && tmp[idx]!=' ')idx++;
  int sz=idx-st+1;
  /* malloc で単語の領域を token に確保 */
  char *token=malloc(sz);
  /* 単語の文字を token の指す領域にコピー */
  strncpy(token,&tmp[st],sz-1);
  *ptr = &tmp[idx]; /* 次の呼び出し用に引数を更新 */
  return token; /* 単語を取り出せた場合 (他で NULL を返す) */
}

char **buildup_arg(char *str) {
  const int N = 10;
  int i = 0, sz = 0;
  char **ptr = NULL;
  char *token = str;
  while (token != NULL) {
    token = gettoken(&str);
    if(i>=sz){
      if(sz==0) sz=N;
      else      sz<<=1;//sz*=2と同じ
      ptr = realloc(ptr,sizeof(char*)*sz);
      if(ptr==NULL) { perror("realloc");exit(1);}
    }
    ptr[i++] = token;
  }
  return ptr;
}

int main(int argc, char *argv[]) {
  size_t sz = 0, n;
  char *str = NULL;
  int cret = 0;
  while(printf("%d>> ", cret)
	&& (n = getline(&str, &sz, stdin)) != -1) {
    if(str[0] == '\n') continue;
    str[n-1]='\0';
    /* fork(),execv(),wait() を使う */
    pid_t pid=fork();
    char** myargv=buildup_arg(str);
    switch (pid) {
    case -1: /* error */
      perror("fork");
      return 1;
    case 0: /* child */
      cret = execv(myargv[0],buildup_arg(str));
      return cret;
    default: /* parent */
      if ( wait( &cret ) == -1 ) perror("wait");
    }
  }
  return 0;
}
