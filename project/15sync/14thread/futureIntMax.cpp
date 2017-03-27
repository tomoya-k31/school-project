#include <iostream>
#include <iomanip>
#include <future>
#include <vector>
#include <climits>
using namespace std;

const int intervals = INT_MAX;
const double width = 1.0 / intervals;

double func(double head,double tail) {
	double sum=0;
	for (int i = head; i < tail; i++ ) {
		const double x = (i + 0.5)*width;
		sum += 1/(1.0 + x*x);
	}
	return sum;
}

int main()
{
	const int num = thread::hardware_concurrency();
	vector<future<double>> a;
	double head = 0;
	for (int i=0; i<num; i++){
		double tail = head + (intervals/num);
		if(i==num-1)tail=intervals;
		a.push_back(async(launch::async, func, head, tail));
		head = tail;
	}
	double sum = 0;
	for(auto& x : a) sum += x.get();
	sum *= 4.0*width;
	cout << "Estimation of pi is "
		<< fixed << setprecision(10)
		<< sum << "\n";
	return 0;
}
