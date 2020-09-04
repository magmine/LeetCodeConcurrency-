#include<thread>
#include<mutex>
#include<condition_variable>
#include <functional>
#include <iostream>

using namespace std;
class Foo {
    std::mutex mtx;
    short flag;
    std::condition_variable cond_var;
public:
    Foo(): flag(1) {
        
    }

    void first(function<void()> printFirst) {
	/**
		No need for locks here since other threads are waiting, and we know that first needs to be first
		:D.
	*/
        printFirst();
        flag = 2;
        cond_var.notify_all();
    }

    void second(function<void()> printSecond) {
        std::unique_lock<std::mutex> lk(mtx);
        cond_var.wait(lk, [this] {return flag == 2;});
        printSecond();
        flag = 3;
        mtx.unlock();
        cond_var.notify_one(); //you only need to notify the third thread
    }

    void third(function<void()> printThird) {
        std::unique_lock<std::mutex> lk(mtx);
        cond_var.wait(lk, [this] {return flag == 3;});
        printThird();
    }
};

int main () {
    Foo foo;
    std::function<void(void)> printFirst = [] {
        std::cout<<"firts"<<"\n";
    };

    std::function<void(void)> printSecond = [] {
        std::cout<<"second"<<"\n";
    };

    std::function<void(void)> printThird = [] {
        std::cout<<"third"<<"\n";
    };

    std::thread t2 = std::thread(&Foo::first, &foo, printFirst);
    std::thread t3 = std::thread(&Foo::second, &foo, printSecond);
    std::thread t1 = std::thread(&Foo::third, &foo, printThird);

    t1.join();
    t2.join();
    t3.join();
    return EXIT_SUCCESS;
}
