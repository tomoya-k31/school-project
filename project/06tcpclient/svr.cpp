// TCPサーバ(反復サーバ)
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>     // for exit()
#include <cstring>     // for strlen()
const unsigned short int port = 63458;      // ポート番号
const int listenq = 2;                      // 待ち受け可能な数

void msg_proc(int);
void error(const char s[]) { perror(s); exit(1); }

int main()
{
   std::cout << "port = " << port << "\n";

   // TCPソケットの作成
   int sockfd = socket(PF_INET, SOCK_STREAM, 0);
   if (sockfd == -1) error("socket");

   // ソケットが使うプロトコルアドレス(IPアドレスとポート)を決める
   sockaddr_in svr;
   memset(&svr, 0, sizeof(svr));             // 全体を０で埋める
   svr.sin_family      = AF_INET;            // アドレスの種類
   svr.sin_addr.s_addr = htonl(INADDR_ANY);  // 相手はどのIPアドレスでも良い
   svr.sin_port        = htons(port);        // ポート番号
   if (bind(sockfd, (sockaddr*)&svr, sizeof(svr)) == -1)
      error("bind");

   // クライアントからの接続要求の受付開始
   if (listen(sockfd, listenq) == -1) error("listen");

   while (true) {
      sockaddr_in cli;
      socklen_t clen = sizeof(cli);
      // 接続のひとつを取り出す
      int fd = accept(sockfd, (sockaddr*)&cli, &clen);
      if (fd < 0 ) error("accept");
      msg_proc(fd);   // acceptで得たfdを使って処理する
      close(fd);      // 終わったら閉じる
   }
}

void msg_proc(int sfd)
{
   std::cout << "msg_proc starts" << "\n";
   while (true) {
      const int bufsz = 128;            // メッセージバッファサイズ
      char buf[bufsz+1];
      int n = recv(sfd, buf, bufsz, 0); // 最大でbufszバイト受信
      if (n == -1 || n == 0) break;
      buf[n] = '\0';
      std::cout << "recv: " << buf << "\n";

      std::stringstream ss;
      ss << strlen(buf) << " bytes";
      std::string s = ss.str();
      if (send(sfd, s.c_str(), s.size(), MSG_NOSIGNAL) == -1){
         error("send"); break;
      }
   }
   std::cout << "msg_proc ends\n";
}
