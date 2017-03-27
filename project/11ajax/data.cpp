#include <iostream>
#include <string>
#include <MyDBS3.hpp>
#include <CGIinput.hpp>
using namespace std;

int main()
{
  CGIinput fmd; // 宣 言 と と も に 入 力 を 終 え る

  if(fmd["flag"] == "post"){
    // 結 果 を 出 力
    cout << "Content-type: text/plain; charset=UTF-8\r\n\r\n";

    // データベースファイルを開く
    MyDBS d("db/tokyo.db");
    if (!d){
      cout << "error";
      return 1;
    }

    string sql;

    // 郵便番号での検索
    sql = "select kanji1,kanji2,kanji3 from post where num = ?;";
    if(d.prepare(sql,fmd["post"]) != SQLITE_OK){
      cout << "error";
      return 1;
    }
    string kanji1,kanji2,kanji3;
    if(d.step(&kanji1,&kanji2,&kanji3) == SQLITE_ROW){
      cout << kanji1 + kanji2 + kanji3;
    }
  }

  if(fmd["flag"] == "fin"){
    
    cout << "Content-type: text/plain; charset=UTF-8\r\n\r\n";

    // データベースファイルを開く
    MyDBS d("db/data.db");
    if (!d){
      cout << "open db error";
      return 1;
    }

    string sql;

    // データの挿入
    sql = "insert into data values (?,?,?,?,?);";
    if(d.exec(sql,fmd["post"],fmd["addr"],fmd["name"],fmd["age"],fmd["gender"]) != SQLITE_OK){
      cout << d.error();
      return 1;
    }
  }

  return 0;
}
