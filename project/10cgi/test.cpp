// CGIプログラムとして働くC++プログラム
// これを動作させるには、
//   ・実行ファイルの拡張子は .cgiとする (例えば test.cgi)
//   ・実行ファイルを~/public_html/cgi-bin にコピーする
//   ・public_html/, cgi-bin/ test.cgi のパーミッション 755
//        % chmod 755 ~/public_html
//        % chmod 755 ~/public_html/cgi-bin/
//        % chmod 755 ~/public_html/cgi-bin/test.cgi
//   ・ホームディレクトリのパーミッション 711
//        % chmod 711 ~
#include <iostream>
using namespace std;

int main()
{
        cout << "Content-type: text/plain\r\n\r\n"; // ヘッダ情報

        char ch;
        while (cin >> noskipws >> ch)
                cout << ch;
	cout << endl;

        return 0;
}
