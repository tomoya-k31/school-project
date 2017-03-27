#include <iostream>
#include <iomanip>
#include <climits>
const int intervals = INT_MAX;
const double width = 1.0 / intervals;
int main(int argc, char *argv[] )
{
	double sum = 0.0;
	for (int i = 0; i < intervals; i++ ) {
		const double x = (i + 0.5)*width;
		sum += 1/(1.0 + x*x);
	}
	sum *= 4.0*width;
	std::cout << "Estimation of pi is "
	<< std::fixed << std::setprecision(10)
	<< sum << "\n";
	return 0;
}
