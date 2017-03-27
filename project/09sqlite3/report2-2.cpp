#include <iostream>
#include <MyDBS3.hpp> // MyDBS3.hppはコンピュータシステム研独自のヘッダファイル
using namespace std;

int main(int argc, char* argv[]){

  if(argc != 2){
    cerr << "Error" << endl;
    return 1;
  }

  // データベースファイルを開く
  MyDBS d("tokyo.db");
  if (!d) {
    cout << d.error() <<"\n";
    return 1;
  }

  // プレースホルダとバインドメカニズムを用いることで
  // 特殊文字を含むすべての文字をただの文字として扱う
  // 住所が一致するものを表示
  string addr = argv[1];
  string sql = "select num from post where kanji1||kanji2||kanji3 = ?;";
  if(d.prepare(sql,addr) != SQLITE_OK){
    cout << d.error() << endl;
    return 1;
  }
  string num;
  while(d.step(&num) == SQLITE_ROW){
    cout << "住所:" << addr << " の郵便番号は " << num << " です。" << endl;
  }

  return 0; // MyDBSのデストラクタがDBを閉じてくれる
}
