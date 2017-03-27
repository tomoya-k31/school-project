#include <iostream>
#include <string>
#include <regex>
using namespace std;

int main() 
{
   wstring s = L"２０１６/１０/１１";

   // exact match
   wregex r1(LR"((.{4})/(.{2})/(.{2}))");
   wsmatch result;
   if (regex_match(s, result, r1)) {
      cout << "match\n";
   }

   return 0;
}
