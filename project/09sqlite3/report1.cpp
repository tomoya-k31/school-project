#include <iostream>
#include <fstream>
#include <MyDBS3.hpp> // MyDBS3.hppはコンピュータシステム研独自のヘッダファイル
using namespace std;

int main(int argc, char* argv[])
{
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

  // ファイルオープン
	ifstream fin(argv[1]);
	if (!fin){
		cerr << "fin" << endl;
		return 1;
	}
  
  // テーブルの初期化
	if (d.exec("delete from post") != SQLITE_OK) {
		cout << d.error() << "\n";
		return 1;
	}

  // トランザクションの開始
	if (d.exec("begin transaction;") != SQLITE_OK) {
		cout << d.error() << "\n";
		return 1;
	}
	
  // データの挿入
	string line;
	while(getline(fin,line)){
		string sql = "insert into post values (" + line + ");";
		if (d.exec(sql) != SQLITE_OK) {
			cout << d.error();
			return 1;
		}

	}

  // トランザクションの終わり
	if (d.exec("commit transaction;") != SQLITE_OK) {
		cout << d.error() << "\n";
		return 1;
	}

	return 0; // MyDBSのデストラクタがDBを閉じてくれる
}
