#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
using namespace std;
int turn = 0;
void func(int id, mutex& mx, condition_variable& cv) {
//	unique_lock<mutex> lock(mx);
//	while (id != turn) cv.wait(lock);
	cout << id << "\n";
	turn ++;
	cv.notify_all(); // bug if it is cv.notify_one();
}
int main() {
	mutex mx;
	condition_variable cv;
	const int num = 10;
	vector<thread> a;
	for (int i = 0; i < num; i++)
		a.push_back(thread(func, i, ref(mx), ref(cv)));
	for (thread &t : a) t.join();
	return 0;
}
