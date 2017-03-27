// MyDBS3.hppはコンピュータシステム研独自のヘッダファイル
// g++ -I/usr/local/include -std=c++11 thisfile.cpp -lsqlite3
// として、sqlite3のライブラリ関数をリンクする
#include <iostream>
#include <MyDBS3.hpp>
using namespace std;

int main(int argc, char* argv[])
{
   // データベースファイルを開く
   MyDBS d("test.db");
   if (!d) {
      cout << d.error() <<"\n";
      return 1;
   }

   // 引数と結果出力がない場合
   if (d.exec("delete from tbl1") != SQLITE_OK) {
      cout << d.error() << "\n";
      return 1;
   }

   // 引数はあるが結果出力のない場合
   // SQL文を作って準備する。ただし、条件などの変化する部分は
   // ?(プレースホルダーと呼ぶ）記号としておき, 後で値を指定する。
   string  s = "insert into tbl1 (id, name) VALUES(?, ?)";
   int id;
   string name;
   while (cin >> id >> name) {
      if (d.exec(s, id, name) != SQLITE_OK) {
         cout << d.error() << "\n";
         break;
      }
   }

   // select文で結果出力を伴う場合（引数はあってもなくても良い）
   s = "select id, name from tbl1 where id >= ?";
   if (d.prepare(s, 100) != SQLITE_OK) 
      cout << d.error() << "\n";
   // selectの出力の順に対応する型の変数のアドレスを指定する
   // 戻り値に注意（タプルがある場合にはSQLITE_ROW, 終了時はSQLITE_DONE）
   while (d.step(&id, &name) == SQLITE_ROW)
      cout << id << ": " << name << "\n";

   return 0; // MyDBSのデストラクタがDBを閉じてくれる
}
