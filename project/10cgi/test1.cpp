#include <iostream>
#include <string>
using namespace std;

int main()
{
        string s = "Content-type: text/plain\r\n\r\n"; // ヘッダ情報

        char ch;
        while (cin >> noskipws >> ch)
                s += ch;
	s += "\n";
	cout << s;

        return 0;
}
