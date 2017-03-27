#include <iostream>
#include <vector>
#include <MyDBS3.hpp> // MyDBS3.hppはコンピュータシステム研独自のヘッダファイル
using namespace std;

// 更新する要素の選択
string set(vector<string> &v){
  cout << "更新する要素の選択と入力\n"; 
  int x;
  string w;
  string sql = " SET";
  cout << "1 : 全国地方公共団体コード\n2 : 郵便番号\n3 : 住所\n番号を入力してください：";
  cin >> x;
  if(x == 1) {
    sql += " code = ?";
    cout << "全国地方公共団体コードの入力 -> ";
  }
  else if(x == 2) {
    sql += " num = ?";
    cout << "郵便番号の入力 -> ";
  }
  else if(x == 3){
    sql += " kanji1 = ?,kana1 = ?,kanji2 = ?,kana2 = ?,kanji3 = ?,kana3 = ?";
    cout << "都道府県名の入力 -> ";cin >> w; v.push_back(w);
    cout << "読み仮名(カナ)の入力 -> ";cin >> w; v.push_back(w);
    cout << "市区町村の入力 -> ";cin >> w; v.push_back(w);
    cout << "読み仮名(カナ)の入力 -> ";cin >> w; v.push_back(w);
    cout << "町域名の入力 -> ";cin >> w; v.push_back(w);
    cout << "読み仮名(カナ)の入力 -> ";
  }
  else {
    cerr << "Error" << endl;
    return "";
  }
  cin >> w;
  v.push_back(w);
  return sql;
}

// 条件の指定
string setWhere(vector<string> &v){
  cout << "条件の入力\n"; 
  int x;
  string w;
  string sql = " WHERE";
  while(cout << "1 : 全国地方公共団体コード\n2 : 郵便番号\n3 : 住所\n番号を入力してください："
      && cin >> x){
    if(x == 1) {
      sql += " code = ?";
      cout << "全国地方公共団体コードの入力 -> ";
    }
    else if(x == 2) {
      sql += " num = ?";
      cout << "郵便番号の入力 -> ";
    }
    else if(x == 3){
      sql += " kanji1||kanji2||kanji3 = ?";
      cout << "住所の入力 -> ";
    }
    else{cerr << "Error" << endl; continue;}
    cin >> w;
    v.push_back(w);
    cout << "1 : AND\n2 : OR\n3 : Quit\n番号を入力してください：";
    cin >> x;
    if(x == 1) sql += " AND";
    else if(x == 2) sql += " OR";
    else if(x == 3) break;
    else cerr << "Error" << endl;
    continue;
  }
  cout << "Quit" << endl;
  return sql;
}

// 最終的なsql文の実行
int execute(MyDBS d,string sql,vector<string> v){
  if(d.prepare(sql) != SQLITE_OK){
    cout << d.error() << endl;
  }
  for(int idx = 0;idx<v.size();idx++){
    if(d.bind(idx+1,v[idx]) != SQLITE_OK){
      cout << d.error() << endl;
      return 1;
    }
  }
  int rc = d.step();
  if(rc != SQLITE_DONE && rc != SQLITE_ROW) {
    cout << d.error() << endl;
    return 1;
  }
  if(d.reset() != SQLITE_OK){
    cout << d.error() << endl;
    return 1;
  }
  return 0;
}

int main(int argc, char* argv[]){

  // データベースファイルを開く
  MyDBS d("tokyo.db");
  if (!d) {
    cout << d.error() << "\n";
    return 1;
  }

  int act;
  cout << "1 : Update\n2 : Delete\n3 : Quit\n操作命令を番号で選択してください：";
  cin >> act;

  string sql;
  vector<string> v; // バインド変数の格納

  switch(act){
    case 1:
      sql = "UPDATE post" + set(v);
      sql += setWhere(v);
      if(execute(d,sql,v))return 1;
      break;

    case 2:
      sql = "DELETE FROM post" + setWhere(v);
      if(execute(d,sql,v))return 1;
      break;

    case 3:
      cout << "Quit" << endl;
      break;

    default:
      cerr << "Error" << endl;
      return 1;
  }

  return 0; // MyDBSのデストラクタがDBを閉じてくれる
}
