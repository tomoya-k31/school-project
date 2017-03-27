// CGIが応答ヘッダにおいてSet-Cookie:を設定するとブラウザはその内容を
// メモリ中に記憶する。さらに、expiresという追加指定があればファイルに保存し、
// ブラウザを終了しても指定された内容を使えるようにする。
// そのブラウザが再度サーバにアクセスした際にはその内容をCookie:で返す。
// Cookieの内容はCGIの環境変数HTTP_COOKIEに入る
///////////////////////////////////
/*
% telnet 192.168.114.15 80
Trying 192.168.114.15...
Connected to 192.168.114.15.
Escape character is '^]'.
GET /~okam/cgi-bin/coo.cgi HTTP/1.0

HTTP/1.1 200 OK
Date: Fri, 11 Jan 2013 07:12:01 GMT
Server: Apache/2.2.15 (Fedora)
Set-Cookie: name=abcdefg; expires=Sat, 11 Jan 2013 16:05:00 JST; Path=/~okam/cgi-bin/
Connection: close
Content-Type: text/plain; charset=UTF-8

cookie test
Connection closed by foreign host.
/////////////////////////////////
% telnet 192.168.114.15 80
Trying 192.168.114.15...
Connected to 192.168.114.15.
Escape character is '^]'.
GET /~okam/cgi-bin/coo.cgi HTTP/1.0
Cookie: name="xyz"

HTTP/1.1 200 OK
Date: Fri, 11 Jan 2013 07:12:39 GMT
Server: Apache/2.2.15 (Fedora)
Set-Cookie: name=abcdefg; expires=Sat, 11 Jan 2013 16:05:00 JST; Path=/~okam/cgi-bin/
Connection: close
Content-Type: text/plain; charset=UTF-8

cookie test
name="xyz"
Connection closed by foreign host.
---------------
NAME=VALUE をセミコロンまたはコンマで区切って指定する。
NAMEやVALUEには、セミコロン、コンマ、空白文字を含まない文字列を指定できる。
Cookieに有効期限をつけるとブラウザ終了後もCookieは保存される。
   expires=Sun, 10-Jun-2001 12:00:00 GMT
有効期限に過去を設定するとブラウザ上のCookieは直ちに削除される
pathを設定すると次回のURLと最長の前方一致したCookieが選ばれる
domainを指定する次回のURLのホストの後方一致でCookieが選ばれる
secureを指定するとhttps通信の時のみCookieが使われる
*/
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
using namespace std;

std::string
get_cookie_time(time_t t)
{
   const int sz = 32; // 30 is enough for this purpose
   char a[sz]; 
   strftime(a, sz, "%a, %d %b %Y %T %Z", localtime(&t));
   return a;
}

int main()
{
  time_t t = time(0);
  string tt = get_cookie_time(t + 60);

   // text/plainを指定していることに注意
   cout << "Content-type: text/plain; charset=UTF-8\r\n"
        << "Set-Cookie: name=abcdefg; "
                "expires=" + tt +"; "
                "Path=/~okam/cgi-bin/"
        << "\r\n\r\n";

   cout << "cookie test\n";
   if (const char * e = getenv("HTTP_COOKIE")) {
      string s = e;
      cout << s << endl;;
   }
   return 0;
}
