#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

std::recursive_mutex locker;

int bst = 0;

void recursion(char c, int loop)
{
    if (loop < 0)
        return;
    locker.lock();
    cout << "Thread : " << c << " " << bst++ << endl;
    recursion(c, --loop);
    locker.unlock();
}
int main()
{
    thread t1(recursion, '0', 10);
    thread t2(recursion, '1', 10);
    t1.join();
    t2.join();
    return 0;
}