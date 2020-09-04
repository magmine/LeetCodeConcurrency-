#include<mutex>
#include<thread>
#include<iostream>
#include<functional>
#include<condition_variable>

using namespace std;

class FooBar {
private:
    int n;
    bool is_foo = true;
    std::mutex mtx;
    std::condition_variable cond_var;

public:
    FooBar(int n) {
        this->n = n;
    }
    
    bool check_true() {
        return is_foo;
    }
    
    bool check_false() {
        return !is_foo;
    }

    void foo(function<void()> printFoo) {
        std::unique_lock<std::mutex> lk(mtx);
        for (int i = 0; i < n; i++) {
            cond_var.wait(lk, [this] {return is_foo;});
        	printFoo();
            is_foo = false;
            mtx.unlock();
            cond_var.notify_one();
        }
    }

    void bar(function<void()> printBar) {
        std::unique_lock<std::mutex> lk(mtx);
        for (int i = 0; i < n; i++) {
            cond_var.wait(lk, [this] {return !is_foo;});
        	printBar();
            is_foo = true;
            mtx.unlock();
            cond_var.notify_one();
        }
    }
};



int main(int argc, char **argv) {
    FooBar fb(atoi(argv[1]));
    std::function<void()> printBar = []() {
        std::cout<<"bar";
    };

    std::function<void()> printFoo = []() {
        std::cout<<"foo";
    };
    std::thread t_foo(&FooBar::foo, &fb, printFoo);
    std::thread t_bar(&FooBar::bar, &fb, []() {
        std::cout<<"bar";
    });
    t_foo.join();
    t_bar.join();
    std::cout<<"\n";
    return 0;
}