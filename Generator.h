#include <coroutine>
#include <exception>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class Generator{
public:
    class promise_type;

    std::coroutine_handle<promise_type> coro;
};

class Generator::promise_type
{
public:

    Generator get_return_object() {
        return {
                .coro = std::coroutine_handle<promise_type>::from_promise(*this)
        };
    }
    auto initial_suspend (){return std::suspend_always();}
    auto final_suspend() noexcept {return std::suspend_always();};
    void return_void(){}
    auto yield_value(const http::response<http::string_body> value){
        current_value = value;
        return std::suspend_always();
    }
    void unhandled_exception(){
        std::terminate();
    }

    http::response<http::string_body> current_value;
};