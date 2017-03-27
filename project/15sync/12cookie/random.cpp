// 8桁のランダムな数字を文字列で返す関数
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>


std::string rand_n_digit()
{
   static std::default_random_engine dre((unsigned)time(0));
   std::uniform_int_distribution<int> di(1,99999999);
   std::stringstream ss;
   ss << std::setfill('0')<< std::setw(8) << di(dre) ;
   return ss.str();
}

int main()
{
   std::cout << rand_n_digit() << "\n";
   std::cout << rand_n_digit() << "\n";
   std::cout << rand_n_digit() << "\n";
}
