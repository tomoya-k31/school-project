#include <iostream>
#include <string>
#include <cstdlib> // for getenv()
using namespace std;

int main()
{
   string s = "Content-type: text/plain\r\n\r\n"; // ヘッダ情報

   if (const char* p = getenv("REQUEST_METHOD")) {
      string method = p;
      s += method + ": ";
      if (method == "GET") {
         if (const char* p2 = getenv("QUERY_STRING"))
            s += p2;
      } else if (method == "POST") {
         char ch;
         while (cin >> noskipws >> ch)
            s += ch;
      }
   }  

   cout << s << endl;;
   return 0;
}
