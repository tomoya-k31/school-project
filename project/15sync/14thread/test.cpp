#include <iostream>
#include <future>
#include <vector>
using namespace std;
double func(double x) { return x*2; }
int main()
{
	const int num = 5;
	vector<future<double>> a;
	for (int i=0; i<num; i++)
		a.push_back(async(launch::async, func, i));
	double sum = 0;
	for(auto& x : a) sum += x.get();
	cout << sum << "\n";
}
