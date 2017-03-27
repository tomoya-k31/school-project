#include <iostream>
using namespace std;

int main()
{
	// text/plainを指定していることに注意
	cout << "Content-type: text/plain; charset=UTF-8\r\n\r\n";
	cout << "情報です";

	return 0;
}
