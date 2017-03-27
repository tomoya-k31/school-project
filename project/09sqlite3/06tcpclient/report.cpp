// TCPクライアント
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>       // for perror()
#include <cstring>      // for memset()
#include <cstdlib>      // for exit()

void msg_proc(int,std::string);
void error(const char s[]) { perror(s); exit(1); }

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <URL> \n";
    return 1;
  }

  // 取得したURLをホストとパスに分解
  std::string url = argv[1];
  std::string host = url.substr(url.find_first_not_of("http://"));
  std::string path = host.substr(host.find_first_of("/"));
  host = host.substr(0,host.find_first_of("/"));

  // 接続先情報の準備
  addrinfo hints, *svr;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;
  hints.ai_flags = AI_NUMERICSERV;
  if (int err = getaddrinfo(host.c_str(), "80", &hints, &svr)) {
    std::cout << "Error: " << gai_strerror(err) << "\n";
    return 1;
  }

  // TCPソケットの作成
  int sockfd = socket(svr->ai_family, SOCK_STREAM, 0);
  if (sockfd < 0) error("socket");

  // 接続
  if (connect(sockfd, svr->ai_addr, svr->ai_addrlen) < 0)
    error("connect");
  freeaddrinfo(svr);   // メモリの開放

  msg_proc(sockfd,path);

  close(sockfd);   // サーバ側に切断を伝える
  return 0;
}

void msg_proc(int sockfd,std::string path)
{
  // 要求の設定
  std::string s;
  s = "GET "+path+" HTTP/1.0\r\n\r\n";

  if (send(sockfd, s.c_str(), s.size(), 0) < 0)
    error("send");
  const int bufsz = 128;// メッセージバッファサイズ
  char buf[bufsz+1];
  std::string sbuf;
  int n=1;
  bool flag = 0;// 空行の読み込みの判別に使用
  while(n){
    n = recv(sockfd, buf, bufsz, 0); // 受信バイト数は最大値のみ指定
    if (n == -1) error("recv");
    buf[n] = '\0';
    sbuf = buf;
    if(!flag && sbuf.find("\r\n\r\n")!=std::string::npos){// 空行の検索
      sbuf=sbuf.substr(sbuf.find("\r\n\r\n")+4);// 存在すれば省き、flagをたてる
      flag=1;
    }
    if(flag) std::cout << sbuf;// 空行以降(html)の内容の出力
  }
  std::cout << std::endl;
}
