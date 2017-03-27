#include <iostream>
#include <string>
#include <regex>
#include <locale>
#include <codecvt>

using namespace std;

int main(int argc, char *argv[])
{
  // 正規表現のパターンやオプションを設定するクラスの宣言
  wregex r1(LR"head(本品は、原材料の一部に)head"); // 頭の部分の解析パターン
  wregex r2(LR"materials(([^、・]+)[・、]?)materials"); // 原材料の解析パターン
  wregex r3(LR"tail(が含まれます。)tail"); // 後ろの部分の解析パターン
  string line;
  while (getline(cin, line)) { // 標準入力を一行ずつ読み取る
    // stringとwstringを相互変換するクラスの宣言
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;
    wstring wline = cv.from_bytes(line); // 読み取った一行をwstring型に変換
    wsmatch result; // マッチした情報を格納するクラスの宣言
    if(regex_search(wline, result, r1)){ // 対象文字列の中にパターンが含まれるか
      // 文字列の不要な頭の部分を省く
      wline = wline.substr(result.position(0)+result.length(0));
      regex_search(wline,result,r3);
      for(wline = wline.substr(0,result.position(0)); // 文字列の不要な後ろの部分を省く
          regex_search(wline, result, r2); // 残った部分をマッチし続けるまで解析
          wline = wline.substr(result.position(0)+result.length(0))) // 文字列の更新
        cout << cv.to_bytes(result[1]) << " "; // マッチした原材料の出力
      cout << endl;
    }
  }
  return 0;
}
