//
// usage: ./a.out '(\d{4})/(\d{2})/(\d{2})' < reg11.cc
//
#include <iostream>
#include <string>
#include <regex>
using namespace std;

int main(int argc, char *argv[])
{
   if (argc < 2) {
      cerr << "usage: " << argv[0] << " <regular-expression> ";
      return 1;
   }

   regex r(argv[1]);
   string line;
   while (getline(cin, line)) {
      smatch result;
      //if (regex_search(line, result, r)) {
      if (regex_match(line, result, r)) {
         cout << line << ": ";
         for (size_t i = 0; i < result.size(); i++)
            cout << result[i] << " ";
         cout << "\n";
      }
   }
   return 0;
}
