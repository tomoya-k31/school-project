#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <string>
#include <MyDBS3.hpp>
#include <CGIinput.hpp>
using namespace std;

int main() {
  CGIinput tbl;
  cout << "Content-type: text/plain; charset=UTF-8\r\n\r\n";
  const char *e = getenv("HTTP_COOKIE");
  if (e != nullptr) {
    // クッキーが送られてきた
    string s = e;
    s = s.substr(5);
    cout << s;
    MyDBS d("db/accountScore.db");
    if(!d) { cout << "open"; return 1; }

    string sql = "select score from accountScore where cookie = ?";
    if(d.prepare(sql,s) != SQLITE_OK){
      cout << "prepare";
      return 1;
    }
    int score;
    if(d.step(&score) != SQLITE_ROW){
      cout << "step";
      return 1;
    }

    if(score<stoi(tbl["score"])){
      string sql = "update accountScore set score = ? where cookie = ?";
      if(d.exec(sql,stoi(tbl["score"]),s) != SQLITE_OK){
        cout << "exec";
        return 1;
      }
      cout << "ok";
    }
    return 0;
  }
  cout << "not"; 
  return 0;
}
