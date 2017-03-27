#include <iostream>
#include <cstdlib>
using namespace std;

int main()
{
    if (const char* s = getenv("HOGE"))
       cout << s << endl;
    else
       cout << "HOGE not found\n";
}
