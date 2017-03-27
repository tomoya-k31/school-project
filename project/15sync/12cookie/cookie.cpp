#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <string>
#include <MyDBS3.hpp>
#include <CGIinput.hpp>
using namespace std;

int html_login(int s=0);
int html_search(string pnum="");
bool check_cookie(string cookie);
bool check_user(string user);
bool check_account(string user,string pass);
void create_account(string user,string pass);
void update_cookie(string user);

int main() {
  CGIinput tbl;
  cout << "Content-type: text/html; charset=UTF-8\r\n";
  const char *e = getenv("HTTP_COOKIE");
  if (e != nullptr) {
    // クッキーが送られてきた
    string s = e;
    bool valid = check_cookie(s); // DBを検索してsを探す
    if (!valid) return html_login(1); // sがない||期限が無効
    string pnum = tbl["post"]; // POSTメソッドのpost指定を取り出す
    return html_search(pnum);
  }
  e = getenv("REQUEST_METHOD");
  if ((e != nullptr && string(e) == "POST")) {

    string user = tbl["user"];
    string pass = tbl["pass"];
    if(tbl["OK"] == "search") return html_login(1); // セッションタイムアウト

    if (user == "" || pass == "") return html_login(2); // form入力エラー

    if (!check_user(user)) create_account(user,pass); // 新アカウント作成
   
    else if (check_account(user,pass)) update_cookie(user); // cookieを更新

    else return html_login(2); // pass間違い

    return html_search(); // 新規またはlogin成功
  }
  return html_login();
}

// login html
int html_login(int s){
  cout << R"(
  <!doctype html>
  <html>
  <head>
  <meta charset="utf-8">
  <title>Login</title>
  </head>
  <body>
  <h1 align="center" style="background-color: #F2F2E5;">ログイン</h1>
  )";

  // 状況に応じた表示
  s==2?cout << R"(<p align="center" style="color: #ff0000;">入力エラー</p>)":
  s==1?cout << R"(<p align="center" style="color: #ff0000;">セッションタイムアウト</p>)":
       cout << R"(<p align="center"">ログインしてください</p>)";

  cout << R"(
  <form action="cookie.cgi" method="post">
  <table align="center">
  <tr>
  <th><label for="user">ユーザー名</label></th>
  <td><input name="user" size="20" type=text /></td>
  </tr>
  <tr>
  <th><label for="pass">パスワード</label></th>
  <td><input name="pass" size="20" type=password /></td>
  </tr>
  <tr>
  <th></th>
  <td align="right"><input type="submit" name="OK" value="login" /></td>
  </tr>
  </table>
  </form>
  </body>
  </html>
  )";
  return 0;
}

// search html and cgi
int html_search(string pnum){
  string search = R"(
  <!doctype html><html>
  <head><meta charset="utf-8"><title>Search</title></head>
  <body><h1 style="background-color: #F2F2E5;">検索システム</h1>
  <h2>住所検索</h2>
  <p>検索したい郵便番号を入力してください。</p>
  <form action="cookie.cgi" method="post">
  <p>郵便番号 <input name="post" size="20" maxlength='7' /></p>
  <input type="submit" name="OK" value="search" />
  </form>
  <h3 style="border-bottom: solid 1px #000000;">検索結果</h3>
  )";

  // データベースファイルを開く
  MyDBS d("db/tokyo.db");
  if (!d) {cout << search+"open error</body></html>";return 1;}

  string sql;

  // 郵便番号での検索
  if(!pnum.empty()){
    sql = "select kanji1,kanji2,kanji3 from post where num = ?;";
    if(d.prepare(sql,pnum) != SQLITE_OK){
      cout << search + d.error() + "</body></html>";
      return 1;
    }
    string kanji1,kanji2,kanji3;
    if(d.step(&kanji1,&kanji2,&kanji3) != SQLITE_ROW){
      search += "<p>郵便番号: 一致するものがありません</p>";
    }
    else{
      search += "<p>郵便番号: " + pnum + " の住所は "
        + kanji1 + kanji2 + kanji3 + " です。</p>";
      while(d.step(&kanji1,&kanji2,&kanji3) == SQLITE_ROW){
        search += "<p>郵便番号: " + pnum + " の住所は "
          + kanji1 + kanji2 + kanji3 + " です。</p>";
      }
    }
  }
  cout << search+"</body></html>";

  return 0;
}

// cookieの有無と期限の確認
bool check_cookie(string cookie){
  MyDBS d("db/account.db");
  if(!d) return 0;

  cookie = cookie.substr(5);

  string sql = "select expire from account where cookie = ?";
  if(d.prepare(sql,cookie) != SQLITE_OK){
    return 0;
  }
  int expire;
  if(d.step(&expire) == SQLITE_ROW){
    return expire > (int)time(0);
  }
  return 0;
}

// userの有無
bool check_user(string user){
  MyDBS d("db/account.db");
  if(!d) return 0;

  string sql = "select count(*) from account where user = ?";
  if(d.prepare(sql,user) != SQLITE_OK){
    return 0;
  }
  int count=0;
  if(d.step(&count) == SQLITE_ROW){
    if(count > 0) return 1;
  }
  return 0;
}

// userとpass対応する2つの有無
bool check_account(string user,string pass){
  MyDBS d("db/account.db");
  if(!d) return 0;

  string sql = "select count(*) from account where user = ? and pass = ?";
  if(d.prepare(sql,user,pass) != SQLITE_OK){
    return 0;
  }
  int count=0;
  if(d.step(&count) == SQLITE_ROW){
    if(count > 0) return 1;
  }
  return 0;
}

// 新しいuserの設定
void create_account(string user,string pass){
  MyDBS d("db/account.db");
  if(!d) return ;
  
  string sql = "insert into account values (?,?,?,?)";
  
  time_t t = time(0);
  t = (time_t)(t+180); // 現時刻から3分後まで有効

  const int sz = 32; // 30 is enough for this purpose
  char e[sz];
  strftime(e, sz, "%a, %d %b %Y %T %Z", localtime(&t));
  string expire = e;
  
  static default_random_engine dre((unsigned)time(0));
  uniform_int_distribution<int> di(1,99999999);
  stringstream ss;
  ss << setfill('0')<< setw(8) << di(dre);
  string cookie = ss.str();

  if(d.exec(sql,user,pass,(int)t+(60*3),cookie) == SQLITE_OK){
    // cookieの設定
    cout << "Set-Cookie: name=" + cookie + ";"
                        "expires=" + expire + ";"
                        "Path=/cgi/"
         << "\r\n";
  }
}

// 既存userの更新
void update_cookie(string user){
  MyDBS d("db/account.db");
  if(!d) return ;
  
  string sql = "update account set expire = ?,cookie = ? where user = ?";
  
  time_t t = time(0);
  t = (time_t)(t+180); // 現時刻から3分後まで有効

  const int sz = 32; // 30 is enough for this purpose
  char e[sz];
  strftime(e, sz, "%a, %d %b %Y %T %Z", localtime(&t));
  string expire = e;

  static default_random_engine dre((unsigned)time(0));
  uniform_int_distribution<int> di(1,99999999);
  stringstream ss;
  ss << setfill('0')<< setw(8) << di(dre);
  string cookie = ss.str();

  if(d.exec(sql,(int)t+(60*3),cookie,user) == SQLITE_OK){
    // cookieの設定
    cout << "Set-Cookie: name=" + cookie + ";"
                        "expires=" + expire + ";"
                        "Path=/cgi/"
         << "\r\n";
  }
}
