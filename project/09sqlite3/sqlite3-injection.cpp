// SQL インジェクションによる脆弱性を持ったプログラム
//
// g++ -I/usr/local/include thisfile.cpp -lsqlite3
// として、sqlite3のライブラリ関数をリンクする
#include <iostream>
#include <MyDBS3.hpp>
using namespace std;

int main()
{
   MyDBS d("passwd.db");
   if (!d) {
      cout << "error\n";
      return 0;
   }

   // userid, passwordにはスペース文字も使える
   string id, ps;
   cout << "userid:";
   getline(cin, id);
   cout << "password:";
   getline(cin, ps);

   string sql = "select userid,secret from secret where userid = \'"
               + id +"\' and passwd = \'" + ps + "\'";
   cout << sql << "\n";

   if (d.prepare(sql) != SQLITE_OK)
      cout << d.error() << "\n";

   cout << d.sql() << "\n";

   string userid;
   int secret;
   while (d.step(&userid, &secret) == SQLITE_ROW)
      cout << userid << " " << secret << "\n";

   return 0;
}
