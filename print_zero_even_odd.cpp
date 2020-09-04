#include<thread>
#include<mutex>
#include<condition_variable>
#include <functional>
#include <iostream>

using namespace std;

class ZeroEvenOdd {
private:
    int it = 1;
    bool is_zero = true;
    int n;
    std::mutex mtx;
    std::condition_variable cv;

public:
    ZeroEvenOdd(int n) {
        this->n = n;
        cv.notify_all();
    }

    void zero(function<void(int)> printNumber) {
        while(true) {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [this]{return is_zero || it > n;});
            is_zero = false;
            if (it > n) {
                lk.unlock();
                cv.notify_all();
                return;
            }
            printNumber(0);
            lk.unlock();
            cv.notify_all();
        }
        
    }

    void even(function<void(int)> printNumber) {
        while(true) {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [this]{return ((!is_zero && it%2 == 0) || it > n);});
            is_zero = true;
            if (it > n) {
                lk.unlock();
                cv.notify_all();
                return;
            }
            printNumber(it);
            it++;
            lk.unlock();
            cv.notify_all();
        }
        
    }

    void odd(function<void(int)> printNumber) {
        while(true) {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [this]{return ((!is_zero && it%2) || it > n);});
            is_zero = true;
            if (it > n) {
                lk.unlock();
                cv.notify_all();
                return;
            }
            printNumber(it);
            it++;
            lk.unlock();
            cv.notify_all();
        }
    }
};

int main() {
    ZeroEvenOdd zeo(15);
    std::function<void(int32_t)> printNumber = [&] (int32_t n) {
        std::cout<<n;
    };
    std::thread t1 = std::thread(&ZeroEvenOdd::zero, std::ref(zeo), printNumber);
    std::thread t2 = std::thread(&ZeroEvenOdd::even, std::ref(zeo), printNumber);
    std::thread t3 = std::thread(&ZeroEvenOdd::odd, std::ref(zeo), printNumber);
    t1.join();
    t2.join();
    t3.join();
}