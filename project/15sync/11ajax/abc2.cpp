#include <iostream>
#include <ctime>
using namespace std;

int main()
{
	// text/plainを指定していることに注意
	cout << "Content-type: text/plain; charset=UTF-8\r\n\r\n";
	time_t t = time(0);             // 現在時刻の取得
	string s = ctime(&t);           // 時刻オブジェクトを文字列に変換
	cout << s.substr(0,s.size()-1); // 末尾の改行文字を削除
	return 0;
}
