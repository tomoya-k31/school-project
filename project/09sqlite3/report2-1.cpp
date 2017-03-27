#include <iostream>
#include <string>
#include <MyDBS3.hpp> // コンピュータシステム研独自のヘッダファイル
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
  
  string postal_code = argv[1];

  // 郵便番号が一致するものを表示する
  string sql = "select kanji1,kanji2,kanji3 from post where num = ?;";
  if(d.prepare(sql,postal_code) != SQLITE_OK){
    cout << d.error() << endl;
    return 1;
  }
  string kanji1,kanji2,kanji3;
  while(d.step(&kanji1,&kanji2,&kanji3) == SQLITE_ROW){
    cout << "郵便番号:" << postal_code <<  "の住所は " 
         << kanji1 << kanji2 << kanji3 << " です。" << endl;
  }

  return 0; // MyDBSのデストラクタがDBを閉じてくれる
}
