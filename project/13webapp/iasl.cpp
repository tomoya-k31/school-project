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
int html_iasl(string user);
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
    return html_iasl(tbl["user"]);
  }
  e = getenv("REQUEST_METHOD");
  if ((e != nullptr && string(e) == "POST")) {

    string user = tbl["user"];
    string pass = tbl["pass"];

    if (user == "" || pass == "") return html_login(2); // form入力エラー

    if (!check_user(user)) create_account(user,pass); // 新アカウント作成
   
    else if (check_account(user,pass)) update_cookie(user); // cookieを更新

    else return html_login(2); // pass間違い

    return html_iasl(tbl["user"]); // 新規またはlogin成功
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
  <form action="iasl.cgi" method="post">
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
int html_iasl(string user){
  cout << R"(
  <!DOCTYPE html><html>
  <head>
  <meta charset="UTF-8">
  <title>iasl app</title>
  <script src="http://code.jquery.com/jquery-3.1.1.min.js"></script>
  <script src="/iasl_ja.js"></script>
  <script src="/iasl_html.js"></script>
  </head>
  <body onLoad='iasl_start()'>
  <div style="position:absolute;left: 500px;">
  <p>your score : <span id='score'>0</span> point</p>
  <table border=1 cellspacing=0 style="border-collapse: collapse;">
  <tr style="background: #ffff99;font-size: 30px;">
  <th colspan=3>ランキング</th>
  </tr>
  <tr>
  <th style="background: #ffcccc;">順位</th>
  <th style="background: #ffcccc;">ユーザー名</th>
  <th style="background: #ffcccc;">スコア</th>
  </tr>
  )";

  MyDBS d("db/accountScore.db");
  if(!d) return 1;

  string sql = "select user,score from accountScore order by score desc";
  if(d.prepare(sql) != SQLITE_OK){
    return 1;
  }
  int count=1;
  string username;
  int score;
  while(d.step(&username,&score) == SQLITE_ROW){
    cout << "<tr>"
      "<td>" << count << "</th>"
      "<td>" + username + "</th>"
      "<td>" << score << "</th>"
      "</tr>";
    if(count++ == 10) break;
  }

  cout << R"(
  </table>
  </div>
  </body>
  </html>
  )";

  return 0;
}

// cookieの有無と期限の確認
bool check_cookie(string cookie){
  MyDBS d("db/accountScore.db");
  if(!d) return 0;

  cookie = cookie.substr(5);

  string sql = "select count(*) from accountScore where cookie = ?";
  if(d.prepare(sql,cookie) != SQLITE_OK){
    return 0;
  }
  int count;
  if(d.step(&count) == SQLITE_ROW){
    if(count > 0) return 1;
  }
  return 0;
}

// userの有無
bool check_user(string user){
  MyDBS d("db/accountScore.db");
  if(!d) return 0;

  string sql = "select count(*) from accountScore where user = ?";
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
  MyDBS d("db/accountScore.db");
  if(!d) return 0;

  string sql = "select count(*) from accountScore where user = ? and pass = ?";
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
  MyDBS d("db/accountScore.db");
  if(!d) exit(2);

  string sql = "insert into accountScore values (?,?,?,?)";

  static default_random_engine dre((unsigned)time(0));
  uniform_int_distribution<int> di(1,99999999);
  stringstream ss;
  ss << setfill('0')<< setw(8) << di(dre);
  string cookie = ss.str();

  if(d.exec(sql,user,pass,0,cookie) == SQLITE_OK){
    // cookieの設定
    cout << "Set-Cookie: name=" + cookie + ";"
      "Path=/cgi/"
      << "\r\n";
  }
}

// 既存userの更新
void update_cookie(string user){
  MyDBS d("db/accountScore.db");
  if(!d) return;

  string sql = "update accountScore set cookie = ? where user = ?";

  static default_random_engine dre((unsigned)time(0));
  uniform_int_distribution<int> di(1,99999999);
  stringstream ss;
  ss << setfill('0')<< setw(8) << di(dre);
  string cookie = ss.str();

  if(d.exec(sql,cookie,user) == SQLITE_OK){
    // cookieの設定
    cout << "Set-Cookie: name=" + cookie + ";"
      "Path=/cgi/"
      << "\r\n";
  }
}
