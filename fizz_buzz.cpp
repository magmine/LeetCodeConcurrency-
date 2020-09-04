#include <mutex>
#include <condition_variable>
#include <iostream>
#include <functional>
#include <thread>
#include <chrono>

using namespace std;

class FizzBuzz {
private:
    int n;
    int it = 1;
    std::mutex mtx;
    std::condition_variable cond_var;
public:
    FizzBuzz(int n) {
        this->n = n;
        cond_var.notify_all();
    }

    void fizz(function<void()> printFizz) {
        while (true) {
            std::unique_lock<std::mutex> lk(mtx);
            cond_var.wait(lk, [this] {
                return ((it % 5 != 0) && (it % 3 == 0)) || it > n;
            });
            if (it <= n) {
                printFizz();
                it++;
                lk.unlock();
                cond_var.notify_all();
            } else {
                lk.unlock();
                cond_var.notify_all();
                return;
            }
        }
        
    }

    void buzz(function<void()> printBuzz) {
        while(true) {
            std::unique_lock<std::mutex> lk(mtx);
            cond_var.wait(lk, [this] {
                return (((it % 5 == 0) && (it % 3 != 0)) || it > n);
            });
            if (it <= n) {
                printBuzz();
                it++;
                lk.unlock();
                cond_var.notify_all();
            } else {
                lk.unlock();
                cond_var.notify_all();
                return;
            }
        }
        
    }

	void fizzbuzz(function<void()> printFizzBuzz) {
        while (true){
            std::unique_lock<std::mutex> lk(mtx);
            cond_var.wait(lk, [this] {
                return ((it % 5 == 0) && (it % 3 == 0)) || it > n;
            });
            if (it <= n) {
                printFizzBuzz();
                it++;
                lk.unlock();
                cond_var.notify_all();
            } else {
                lk.unlock();
                cond_var.notify_all();
                return;
            }
        }
    }

    void number(function<void(int)> printNumber) {
        while (true) {
            std::unique_lock<std::mutex> lk(mtx);
            cond_var.wait(lk, [this] {
                return ((it % 5 != 0) && (it % 3 != 0)) || it > n;
            });
            if (it <= n) {
                printNumber(it);
                it++;
                lk.unlock();
                cond_var.notify_all();
            } else {
                lk.unlock();
                cond_var.notify_all();
                return;
            }
        }
    }
};

int main() {
    FizzBuzz fizzbuzz(15);
    std::function<void(void)> printFizz = [&] {
        std::cout<<"Fizz"<<"\n";
    };

    std::function<void(void)> printBuzz = [&] {
        std::cout<<"Buzz"<<"\n";
    };

    std::function<void(void)> printFizzBuzz = [&] {
        std::cout<<"FizzBuzz"<<"\n";
    };

    std::function<void(int32_t)> printNumber = [&] (int32_t n) {
        std::cout<<std::to_string(n)<<"\n";
    };
    std::thread t1 = std::thread(&FizzBuzz::fizz, std::ref(fizzbuzz), printFizz);
    std::thread t2 = std::thread(&FizzBuzz::buzz, std::ref(fizzbuzz), printBuzz);
    std::thread t3 = std::thread(&FizzBuzz::fizzbuzz, std::ref(fizzbuzz), printFizzBuzz);
    std::thread t4 = std::thread(&FizzBuzz::number, std::ref(fizzbuzz), printNumber);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}