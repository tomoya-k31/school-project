#include <iostream>
#include <cstdlib>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
using namespace std;

int worker = 0;
int count = 0;
mutex f;
bool is_prime(int x) {
  for (int i = 3; i*i <= x; i+=2 )
    if (x % i == 0) return false;
  return true;
}

void prime_count(int n,const int p,int max,condition_variable &cv){
	int fcount = 0;
  if(n==1){ // 1を省き2の分加算
    n+=p;
    if(max>=2) fcount++;
  }
	for (int i = n; i <= max; i+=p) // 素数は奇数のみ判定
    if (is_prime(i)) fcount++;
	unique_lock<mutex> l(f);
	count += fcount;
	if(--worker == 0) cv.notify_one();
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "usage: "<<argv[0]<<" max\n";
		return 1;
	}
	int max = atoi(argv[1]);
	if (max <= 0) return 1;
	const int num = thread::hardware_concurrency();
	worker = num;
	mutex mx;
	condition_variable cv;
	auto start = std::chrono::high_resolution_clock::now();
	for (int i=0; i<num; i++){
		thread(prime_count,i*2+1,num*2,max,ref(cv)).detach();
	}
	unique_lock<mutex> lock(mx);
	while(worker > 0) cv.wait(lock);
	auto stop = std::chrono::high_resolution_clock::now();
	cout << "# of prime (<="<<max<<") is "<<count<<"\n";
	std::cout << "It took "
		<< std::chrono::duration<double>(stop-start).count()
		<< " seconds\n";
	return 0;
}
