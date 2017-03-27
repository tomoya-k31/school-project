#include <iostream>
#include <MyDBS3.hpp>
#include <CGIinput.hpp>
using namespace std;

int error(string e,string str){
	e += str;
	cout << e;
	return 1;
}

int main()
{
	CGIinput fmd; // 宣 言 と と も に 入 力 を 終 え る
	//結 果 を 出 力
	string s = "Content-type: text/html\r\n\r\n";
	// HTML を 返 す 時 の タ イ プ
	s += R"(
		<!doctype html><html>
		<head><meta charset=\"utf-8\"></head>
		<body><h1>report test</h1>
		<form action="db.cgi" method="post">
		<p>メッセージ1: <input name="message1" size="60" /> </p>
		<input type="submit" name="OK" value="OKay" />
		</form>
		)";

		MyDBS d("db/tokyo.db");
	if (!d) return error(s,"db error");
	string sql = "select kanji1,kanji2,kanji3 from post where num = ?;";
	if(d.prepare(sql,fmd["message1"]) != SQLITE_OK){
		return error(s,d.error());
	}
	string kanji1,kanji2,kanji3;
	while(d.step(&kanji1,&kanji2,&kanji3) == SQLITE_ROW){
		s+=kanji1+kanji2+kanji3;
	}
	s += "</body></html>";

	cout << s;

	return 0;
}
