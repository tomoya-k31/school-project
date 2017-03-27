// CGIプログラムとして働くC++プログラム
// フォームデータを解析して簡単に使えるようにする
#include <iostream>
#include <string>
#include <map>       // map<string, string>用
#include <cctype>    // getnum内で呼び出す関数用
using namespace std;

// 16進数表記の2文字の列c1c2を対応する数値に変換する
int getnum(char c1, char c2)
{
   int a = isdigit(c1) ? (c1 - '0')      :
           isupper(c1) ? (c1 - 'A' + 10) :
           islower(c1) ? (c1 - 'a' + 10) : 0;
   int b = isdigit(c2) ? (c2 - '0')      :
           isupper(c2) ? (c2 - 'A' + 10) :
           islower(c2) ? (c2 - 'a' + 10) : 0;
   return 16*a + b;
}

// 文字列中の特殊文字を変換する
string decode(string s)
{
   string x;
   for (size_t i = 0; i < s.size(); i++) {
      switch (s[i]) {
      case '+':
         x += " ";
         break;
      case '%':
         if (i+2 < s.size()) {
            x += getnum(s[i+1],s[i+2]);
            i += 2;
            break;
         }
      default:  x += s[i];
      }
   }
   return x;
}

// 標準入力からの文字列を解析して、フォームデータを
// map型の変数に入れる
map<string, string> read_and_parse()
{
   map<string, string> env;
   string line;
   while (getline(cin, line, '&')) {
      if (line.size()==0) continue;
      size_t p = line.find('=');
      if (p == string::npos) {
         env[line] = "";        // 登録
         continue;
      }
      string key   = decode(line.substr(0,p));
      string value = decode(line.substr(p+1));
      env[key] = value;        // 登録
    }
    return env;
}

int main()
{
    map<string, string> fmd = read_and_parse();

    string s =
         "Content-type: text/html\r\n\r\n"       // HTMLを返す時のタイプ
         "<!doctype html><html>"
         "<head><meta charset=\"utf-8\"></head>"
         "<body><h1>cgi test</h1>";
    s += "message1 ->" + fmd["message1"] + "<br>";
    s += "message2 ->" + fmd["message2"] + "<br>";
    s += "</body></html>";
    cout << s;
    return 0;
}
