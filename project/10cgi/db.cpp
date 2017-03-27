#include <iostream>
#include <MyDBS3.hpp>
#include <CGIinput.hpp>
using namespace std;

// エラー処理
int error(string e,string str){
  e += str;
  cout << e + "</body></html>";
  return 1;
}

int main()
{
  CGIinput fmd; // 宣 言 と と も に 入 力 を 終 え る

  // 結 果 を 出 力
  string s = "Content-type: text/html\r\n\r\n";

  // HTML を 返 す 時 の タ イ プ
  s += R"(
<!doctype html><html>
<head><meta charset="utf-8"><title>report</title></head>
<body><h1 style="background-color: #F2F2E5;">検索システム</h1>
<h2>住所検索</h2>
<p>検索したい住所、または郵便番号を入力してください。</p>
<form action="db.cgi" method="post">
<p>郵便番号: <input name="post" size="20" /> ex.123-4567 → 1234567</p>
<p>　住所　: <input name="addr" size="20" /> ex.東京都○○区△△△</p>
<input type="submit" name="OK" value="search" />
</form>
<h3 style="border-bottom: solid 1px #000000;">検索結果</h3>
)";

  // データベースファイルを開く
  MyDBS d("db/tokyo.db");
  if (!d) return error(s,"db error");

  string sql;

  // 郵便番号での検索
  if(!fmd["post"].empty()){
    sql = "select kanji1,kanji2,kanji3 from post where num = ?;";
    if(d.prepare(sql,fmd["post"]) != SQLITE_OK){
      return error(s,d.error());
    }
    string kanji1,kanji2,kanji3;
    if(d.step(&kanji1,&kanji2,&kanji3) != SQLITE_ROW){
      s += "<p>郵便番号: 一致するものがありません</p>";
    }
    else{
      s += "<p>郵便番号: " + fmd["post"] + " の住所は "
        + kanji1 + kanji2 + kanji3 + " です。</p>";
      while(d.step(&kanji1,&kanji2,&kanji3) == SQLITE_ROW){
        s += "<p>郵便番号: " + fmd["post"] + " の住所は "
          + kanji1 + kanji2 + kanji3 + " です。</p>";
      }
    }
  }

  // 住所での検索
  if(!fmd["addr"].empty()){
    sql = "select num from post where kanji1||kanji2||kanji3 = ?;";
    if(d.prepare(sql,fmd["addr"]) != SQLITE_OK){
      return error(s,d.error());
    }
    string num;
    if(d.step(&num) != SQLITE_ROW){
      s += "<p>　住所　: 一致するものがありません</p>";
    }
    else{
      s += "<p>　住所　: " + fmd["addr"] + " の郵便番号は " + num + " です。</p>";
      while(d.step(&num) == SQLITE_ROW){
        s += "<p>　住所　: " + fmd["addr"] + " の郵便番号は " + num + " です。</p>";
      }
    }
  }

  s += "</body></html>";
  cout << s;

  return 0;
}
