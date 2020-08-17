//vs2015
#include <iostream>
#include <string>
#include <future>
#include <exception>
#include <experimental/coroutine>
using namespace std;

template <class T>
struct test
{
    // inner types
    struct promise_type;
    using handle_type = std::experimental::coroutine_handle<promise_type>; //type alias

    // functions
    test(handle_type h) : handle(h) {
        cout << "# Created a Test object\n";
    }
    test(const test &s) = delete;
    test &operator=(const test &) = delete;
    test(test &&s) : handle(s.handle) {
        s.handle = nullptr;
    }
    test &operator=(test &&s)
    {
        handle = s.handle;
        s.handle = nullptr;
        return *this;
    }
    ~test()
    {
        cout << "#Test gone\n";
        if (handle)
            handle.destroy();
    }

    T get()
    {
        cout << "# Got return value\n";
        if (!(this->handle.done()))
        {
            handle.resume(); //resume
            return handle.promise().value;
        }
        else
            throw runtime_error("no object for getting");
    }

    struct promise_type
    {
        promise_type() {
            cout << "@ promise_type created\n";
        }
        ~promise_type() {
            cout << "@ promise_type died\n";
        }

        auto get_return_object() //get return object
        {
            cout << "@ get_return_object called\n";
            return test<T>{handle_type::from_promise(*this)}; // pass handle to create "return object"
        }

        auto initial_suspend() // called before run coroutine body
        {
            cout << "@ initial_suspend is called\n";
            // return std::experimental::suspend_never{}; // dont suspend it
            return std::experimental::suspend_always{};
        }

        auto return_value(T v) // called when there is co_return expression
        {
            cout << "@ return_value is called\n";
            value = v;
            return std::experimental::suspend_never{}; // dont suspend it
                                                       //return std::experimental::suspend_always{};
        }

        auto final_suspend() // called at the end of coroutine body
        {
            cout << "@ final_suspend is called\n";
            return std::experimental::suspend_always{};
        }

        void unhandled_exception() //exception handler
        {
            std::exit(1);
        }

        // data
        T value;
    };

    // member variables
    handle_type handle;
};

test<int> coroutine01()
{
    std::cout << "start coroutine01\n";
    co_return 1;
    co_return 2; // will never reach here
}

int main()
{
    {
        auto a = coroutine01();
        cout << "created a corutine, try to get a value\n";
        int an = a.get();
        cout << "value is " << an << endl;
        an = a.get();
        cout << "value is " << an << endl;
    }
    return 0;
}

// std::future<std::expected<std::string>> load_data(std::string resource)
// {
//     auto handle = co_await open_resouce(resource);
//     while (auto line = co_await read_line(handle)) {
//         if (std::optional<std::string> r = parse_data_from_line(line))
//             co_return *r;
//     }
//     co_return std::unexpected(resource_lacks_data(resource));
// }