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

void msg_proc(int);
void error(const char s[]) { perror(s); exit(1); }

int main(int argc, char *argv[])
{
   if (argc < 3) {
      std::cerr << "usage: " << argv[0] << " <IP-Address> <PortNum>\n";
      return 1;
   }

   // 接続先情報の準備
   addrinfo hints, *svr;
   memset(&hints, 0, sizeof(hints));
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_family = AF_INET;
   hints.ai_flags = AI_NUMERICSERV;
   if (int err = getaddrinfo(argv[1], argv[2], &hints, &svr)) {
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

   msg_proc(sockfd);

   close(sockfd);   // サーバ側に切断を伝える
   return 0;
}
 
void msg_proc(int sockfd)
{
   std::string s;
   while (std::cout << "input:" && std::cin >> s)  {
       s += "\r\nHost: aaa\r\n\r\n";
       if (send(sockfd, s.c_str(), s.size(), 0) < 0)
          error("send");

       std::cout << "waiting...\n";
       const int bufsz = 128;               // メッセージバッファサイズ
       char buf[bufsz+1];
       int n = recv(sockfd, buf, bufsz, 0); // 受信バイト数は最大値のみ指定
       if (n == -1) error("recv");
       buf[n] = '\0';
       std::cout << buf << "\n";
   }
}
