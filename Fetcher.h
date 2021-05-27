//
// Created by padlo on 27.05.2021.
//

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class Fetcher : public std::enable_shared_from_this<Fetcher>{

public:
    explicit Fetcher(boost::asio::io_context &io);
    void run( char const* host, char const* port, char const* target, int version);

private:
    void printError(beast::error_code error, char const* what);

    void on_resolve(beast::error_code error, tcp::resolver::results_type results);
    void on_connect(beast::error_code error, tcp::resolver::results_type::endpoint_type);
    void on_write( beast::error_code error, std::size_t bytes_transferred);
    void on_read(beast::error_code error, std::size_t bytes_transferred);

    tcp::resolver resolver_; //To resolve the adress and get ip
    beast::tcp_stream stream_; // For connecting to ip
    beast::flat_buffer buffer_;
    http::request<http::empty_body> req_;
    http::response<http::string_body> res_;
};
