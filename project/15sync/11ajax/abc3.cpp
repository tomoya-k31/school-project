#include <iostream>
using namespace std;

int main()
{
	// text/plainを指定していることに注意
	cout << "Content-type: text/plain; charset=UTF-8\r\n\r\n";
 	for (char ch; cin >> noskipws >> ch; ) cout << ch;  
	return 0;
}
