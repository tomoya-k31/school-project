#include <iostream>
#include <cstdlib>
using namespace std;
bool is_prime(int x) {
	if (x == 2) return true;
	if (x % 2 == 0) return false;
	for (int i = 3; i*i <= x; i+=2 )
		if (x % i == 0) return false;
	return true;
}
int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "usage: "<<argv[0]<<" max\n";
		return 1;
	}
	int max = atoi(argv[1]);
	if (max <= 0) return 1;
	int count = 0;
	for (int i = 2; i <=max; i++) // prime number is greater than 1
		if (is_prime(i)) ++ count;
	cout << "# of prime (<="<<max<<") is "<<count<<"\n";
	return 0;
}
