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

   // sql文を作って準備する。ただし、条件などの変化する部分は
   // :文字列 で引数を登録しておき、後で値を指定する。
   string s = "insert into tbl1 (id, name) VALUES(:id, :name)";
   if (d.prepare(s) != SQLITE_OK) {
      cout << d.error() << "\n";
      return 1;
   }

   int id;
   string name;
   while (cin >> id >> name) {
      if (d.bind(":id",   id)   != SQLITE_OK ||
          d.bind(":name", name) != SQLITE_OK) {
         cout << d.error() << "\n";
         break;
      }

      int rc = d.step();
      if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
         cout << d.error() << "\n";
         break;
      }
      if (d.reset() != SQLITE_OK) {
         cout << d.error() << "\n";
         break;
      }
   }

   s = "select * from tbl1 order by id";
   if (d.prepare(s) != SQLITE_OK) 
      cout << d.error() << "\n";
   while (d.step(&id, &name) == SQLITE_ROW)
       cout << id << ": " << name << "\n";

   return 0; // MyDBSのデストラクタがDBを閉じてくれる
}
