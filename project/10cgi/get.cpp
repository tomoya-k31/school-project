// CGIとして働くC++プログラム
// Trying 133.220.114.232...
// Connected to csweb.
// Escape character is '^]'.
// GET /~okam/cgi-bin/get.cgi?a=1&b=3 HTTP/1.1
// Host: hoge
// 
// HTTP/1.1 200 OK
// Date: Thu, 03 Dec 2009 06:30:46 GMT
// Server: Apache/2.2.13 (Fedora)
// Content-Length: 19
// Connection: close
// Content-Type: text/html
// 
// <pre>a=1&b=3
// </pre>Connection closed by foreign host.
// 
#include <iostream>
#include <cstdlib>
using namespace std;

int main()
{
        char ch;

        cout << "Content-type: text/html\r\n\r\n";
        cout << "<pre>";

        string q = getenv("QUERY_STRING");;
        if (q != "") cout << q << endl;

        cout << "</pre>";

        return 0;
}
