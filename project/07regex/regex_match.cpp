#include <iostream>
#include <string>
#include <regex>
using namespace std;

int main() 
{
   string s = "2012/04/01";

   // exact match
   regex r1(R"((\d{4})/(\d{2})/(\d{2}))");
   smatch result;
   if (regex_match(s, result, r1)) {
      for (size_t i = 0; i < result.size(); i++)
         cout << result[i] << " ";
      cout << "\n";
   }

   // search
   regex r2(R"((\d{2})/(\d{2}))");
   if (regex_search(s, result, r2)) {
      for (size_t i = 0; i < result.size(); i++)
         cout << result[i] << " ";
      cout << "\n";
   }
   return 0;
}
