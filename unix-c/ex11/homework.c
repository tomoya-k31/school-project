/*
 *  ex11-homework.c created by S142063
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

int stat(const char *pathname, struct stat *buf);

void dirwalk(const char* dname, int level) {
  // カレントディレクトリの移動
  chdir(dname);

  // ディレクトリオープン
  DIR *dfd = opendir(".");
  if(!dfd){
    perror("opendir");
    return ;
  }

  struct stat buf;

  struct dirent *dp;
  while((dp = readdir(dfd))){ // ディレクトリの中身を読みだす
    // "."と".."の除外
    if (!strcmp(dp->d_name,".") || !strcmp(dp->d_name,"..")) continue;
    // statによるファイルのメタ情報を取得
    if (stat(dp->d_name, &buf) == -1) {
      perror("stat"); continue;
    }
    // ファイル種別を確認
    if ((buf.st_mode & S_IFMT) != S_IFDIR) continue; // ディレクトリ以外は除く
    for (int i = 0; i < level; i++) putchar(' ');
    printf("%s\n", dp->d_name); // ディレクトリ名を出力
    dirwalk(dp->d_name, level+1); // 取得したディレクトリで再帰処理
  }

  closedir(dfd); // ディレクトリを閉じる

  // カレントディレクトリの移動
  chdir("..");
}

int main(int argc, char *argv[])
{
  if(argc < 2){
    printf("usage %s dir name\n",argv[0]);
    return 1;
  }
  
  // 指定ディレクトリの出力
  printf("%s\n",argv[1]);

  // 指定ディレクトリの子孫のディレクトリ名の出力
  dirwalk(argv[1],1);

  return 0;
}
