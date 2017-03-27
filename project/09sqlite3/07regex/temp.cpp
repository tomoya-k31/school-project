#include <iostream>
#include <string>
#include <regex>
#include <codecvt>
#include <locale>

using namespace std;

int main(int argc, char *argv[])
{
  // 正規表現のパターンやオプションを設定するクラスの宣言
  // 不要な部分を省くための解析パターン
  wregex r1(LR"(本品は、原材料の一部に(.*)が含まれます。)");
  wregex r2(LR"(([^、・]+)[、・]?(.*))"); // 原材料の解析パターン
  string line;
  while (getline(cin, line)) { // 標準入力を一行ずつ読み取る
    // stringとwstringを相互変換するクラスの宣言
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;
    wstring wline = cv.from_bytes(line); // 読み取った一行をwstring型に変換
    wsmatch result; // マッチした情報を格納するクラスの宣言
    if (regex_search(wline, result, r1)) { // r1の解析パターンを検索
      wline = result[1]; // 不要な部分を省き原材料のみに文字列に更新
      while(regex_search(wline,result,r2)){ // r2の解析パターンを繰り返し検索
        cout << cv.to_bytes(result[1]) << " "; // 見つかった原材料の一つを出力
        wline = result[2]; // 残りの原材料に更新
      }
      cout << endl;
    }
  }
  return 0;
}
