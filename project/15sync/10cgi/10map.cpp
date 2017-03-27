#include <map>
#include <string>
#include <iostream>
using std::cout;

int main()
{
   std::map<std::string,int> cities = {
             {"Tokyo",37237000}, {"Jakarta",26746000}, {"Seoul",22868000}, 
             {"Delhi",22826000}, {"Shanghai",21766000},{"Manila",21241000}
   };

   // 連想記憶として
   cout <<"Tokyo:" << cities["Tokyo"] << "\n";

   cities["Musashino"] = 138700;
   cout <<"Musashino:" << cities["Musashino"] << "\n";

   // ABC順に出力
   for (const auto& x : cities)
      cout << x.first <<" -> "<< x.second <<"\n";
}
