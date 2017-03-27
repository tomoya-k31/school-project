#include <iostream>
#include <iomanip>
#include <locale> 
#include <codecvt> 


int main()
{
   //std::string org = u8"今日は朝から雨だった。";
   std::string org = "今日は朝から雨だった。";
   std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;

   //string -> wstring
   std::wstring wstr = cv.from_bytes(org);

   //wstring -> string
   std::string rev = cv.to_bytes(wstr);
   std::cout << std::boolalpha << (org == rev) <<"\n";
   std::cout << rev << std::endl;

   wstr[6] = L'雪'; // wide char literal
   std::cout << cv.to_bytes(wstr) << std::endl;

   return 0;
}
