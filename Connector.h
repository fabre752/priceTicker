#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <coroutine>
#include <boost/beast/ssl.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;


class Connector{
public:
    Connector(net::io_context &ioc, ssl::context &ctx);
    awaitable<void> establish_connection(char const* host, char const* port, char const* target, int version);
private:
    awaitable<void> perform_scan();
    void run_queries();
    void write_out();
    tcp::resolver resolver;
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    beast::ssl_stream<beast::tcp_stream> stream;
    beast::flat_buffer buffer;
};
