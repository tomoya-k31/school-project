/*
 * created by us142063
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char* gettoken(char **ptr) {
  char *tmp = *ptr;  /* tmp[idx]の形で使用する*/
  int idx = 0;
  if(!tmp || tmp[idx] =='\0') return NULL; /* error check */
  while(tmp[idx] == ' ') idx++;  /*  先頭のスペースを読み飛ばす*/
  int head = idx;
  while(tmp[idx] != ' ' && tmp[idx] != '\0') idx++; /*  単語の終わりを見つける*/
  if(head == idx) return NULL;
  char *token = malloc(sizeof(char)*(idx-head+1)); /* malloc  で単語の領域をtokenに確保*/
  if(!token){ perror("malloc"); return NULL; }
  strncpy(token,&tmp[head],idx-head); /*  単語の文字を  token  の指す領域にコピー*/
  *ptr = &tmp[idx]; /*  次の呼び出し用に引数を更新  */
  return token; /*  単語を取り出せた場合(  他で  NULL  を返す  ) */
}

char **buildup_arg(char *str) {
  const int N = 10;
  int i = 0, sz = 0;
  char **ptr = NULL;
  char *token = str;
  while (token != NULL) {
    token = gettoken(&str);
    if(i >= sz){
      if(sz == 0) sz = N;
      else        sz = sz*2;
      ptr = realloc(ptr,sizeof(char*)*sz); /* realloc の例を参考にする*/
      if(!ptr) { perror("realloc"); exit(1); } /* error check */
    }
    ptr[i++] = token;
  }
  return ptr;
}

int main(int argc, char *argv[]) {
  size_t sz = 0, n;
  char *str = NULL;
  int cret = 0;
  char **e;
  while (printf("%d>> ", cret)
      && (n = getline(&str, &sz, stdin)) != -1) {
    if (str[0] == '\n') continue;
    /* fork(),execv(),wait()を使う*/
    pid_t pid = fork();
    switch (pid) {
      case -1: /* error */
        perror("fork");
        return 1;
      case 0:  /* child */
        str[n-1] = '\0';
        e = buildup_arg(str);
        execv(e[0], e);
        perror("execv");
        return 1;
      default: /* parent */
        if ( wait( &cret ) == -1 ) perror("wait");
    }
  }
  printf("\n");
  return 0;
}
