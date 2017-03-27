// 指定時刻をcookieで利用できる形式の文字列で返す関数
#include <iostream>
#include <string>
#include <ctime>

std::string
get_cookie_time(time_t t)
{
   const int sz = 32; // 30 is enough for this purpose
   char a[sz]; 
   strftime(a, sz, "%a, %d %b %Y %T %Z", localtime(&t));
   return a;
}

int main()
{
  time_t t = time(0);
  std::cout << get_cookie_time(t + 0) << "\n"; 
  std::cout << get_cookie_time(t + 3600) << "\n"; 
  std::cout << get_cookie_time(t + 3600*24) << "\n"; 
}
