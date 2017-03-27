#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <climits>
using namespace std;

const int intervals = 40000000;
const double width = 1.0 / intervals;

double func(double head,double tail,double &sum) {
	sum = 0.0;
	for (int i = head; i < tail; i++ ) {
		const double x = (i + 0.5)*width;
		sum += 1/(1.0 + x*x);
	}
}

int main()
{
	const int num = thread::hardware_concurrency();
	vector<thread> a;
	vector<double> total(num);
	for (int i=0; i<num; i++){
		double tmp = i*(intervals/num);
		a.push_back(thread(func, tmp, tmp+(intervals/num),ref(total[i])));
	}
	double sum = 0.0;
	for(auto& x : a) x.join();
	for (double &s : total) sum += s;
	sum *= 4.0*width;
	cout << "Estimation of pi is "
	     << fixed << setprecision(10)
	     << sum << "\n";
	return 0;
}
