#include <iostream>
#include <iomanip>
#include <future>
#include <vector>
#include <climits>
using namespace std;

const int intervals = 40000000;
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
	for (int i=0; i<num; i++){
		double tmp = i*(intervals/num);
		a.push_back(async(launch::async, func, tmp, tmp+(intervals/num)));
	}
	double sum = 0;
	for(auto& x : a) sum += x.get();
	sum *= 4.0*width;
	cout << "Estimation of pi is "
	     << fixed << setprecision(10)
	     << sum << "\n";
	return 0;
}
