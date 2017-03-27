// CGIプログラムとして働くC++プログラム
// フォームデータを解析して簡単に使えるようにする
// GETメソッド/POSTメソッドのどちらでも対応する
#include <CGIinput.hpp>
using namespace std;

int
main()
{
    CGIinput fmd; // 宣言とともに入力を終える

    // 結果を出力
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
