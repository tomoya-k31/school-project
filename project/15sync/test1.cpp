#include <iostream>
#include <thread>
#include <mutex>
using namespace std;
const int inittotal = 500000;
int checking = 0, savings = inittotal;
mutex mx;
void savings_to_checking(int amount) {
	lock_guard<mutex> lg(mx);
	savings -= amount;
	checking += amount;
}
int total_balance() {
	lock_guard<mutex> lg(mx);
	int balance = checking + savings;
	return balance;
}
void func() {
	for (int i = 0; i<inittotal; i++)
		savings_to_checking(1);
}
int main() {
	thread t(func);
	for (int i = 0; i<inittotal; i++)
		if (total_balance() != inittotal)
			cerr << "error\n";
	t.join();
	return 0;
}
