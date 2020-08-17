#include <iostream>
#include <chrono>
#include <thread>
#include <experimental/coroutine>

using namespace std;

struct Task
{
    struct promise_type
    {
        auto get_return_object() { return Task{}; }
        auto initial_suspend() { return std::experimental::suspend_never{}; }
        auto final_suspend() { return std::experimental::suspend_never{}; }
        void unhandled_exception() { std::terminate(); }
        void return_void() {}
    };
};

using call_back = std::function<void(int)>;
void Add100ByCallback(int init, call_back f)
{
    std::thread t([init, f]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        f(init + 100);
    });

    t.detach();
}

struct Add100AWaitable
{
    Add100AWaitable(int init) : init_(init) {}
    bool await_ready() const { return false; }
    int await_resume() { return result_; }
    void await_suspend(std::experimental::coroutine_handle<> handle)
    {
        // 定义一个回调函数，在此函数中恢复协程
        auto f = [handle, this](int value) mutable {
            result_ = value;
            handle.resume(); // 这句是关键
        };
        Add100ByCallback(init_, f);
    }
    int init_;   // 将参数存在这里
    int result_; // 将返回值存在这里
};

Task Add100ByCoroutine(int init, call_back f)
{
    int ret = co_await Add100AWaitable(init);
    ret = co_await Add100AWaitable(ret);
    ret = co_await Add100AWaitable(ret);
    f(ret);    
}

int main(int argc, const char *argv[])
{
    const char8_t *USD = u8"USD", *EUR = u8"EUR";


    auto t = Add100ByCoroutine(10, [](int ret) { cout << "ret = " << ret << endl; });
    getchar();
}