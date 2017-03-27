//
// retrieve url of img tag
//
#include <iostream>
#include <string>
#include <regex>
using namespace std;

int main(int argc, char *argv[])
{
   /*if (argc < 2) {
      cerr << "usage: " << argv[0] << " <regular-expression> ";
      return 1;
   }*/

   // <img .... src="....."...>
   regex r(R"(<img\s+src=\"([^\"]+)\")");
   string line;
   while (getline(cin, line)) {
      smatch result;
      if (regex_search(line, result, r)) {
         cout << result[1] << "\n";
      }
   }
   return 0;
}
