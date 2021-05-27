#include "Fetcher.h"

Fetcher::Fetcher(net::io_context& ioc) : resolver_(net::make_strand(ioc)), stream_(net::make_strand(ioc))
{
}

void Fetcher::printError(beast::error_code error, char const* what)
{
    std::cerr << what << ": " << error.message() << "\n";
}

void Fetcher::run( char const* host, char const* port, char const* target, int version)
{
    // Set up an HTTP GET request message
    req_.version(version);
    req_.method(http::verb::get);
    req_.target(target);
    req_.set(http::field::host, host);

    // Resolving domain name
    resolver_.async_resolve( host, port,beast::bind_front_handler( &Fetcher::on_resolve, shared_from_this()));
}

void Fetcher::on_resolve(beast::error_code error, tcp::resolver::results_type results)
{
    if(error)
        return printError(error, "resolve");

    stream_.expires_after(std::chrono::seconds(30));
    stream_.async_connect( results,beast::bind_front_handler( &Fetcher::on_connect, shared_from_this()));
}

void Fetcher::on_connect(beast::error_code error, tcp::resolver::results_type::endpoint_type)
{
    if(error)
        return printError(error, "connect");

    stream_.expires_after(std::chrono::seconds(30));
    //Sending HTTP request
    http::async_write(stream_, req_,beast::bind_front_handler( &Fetcher::on_write, shared_from_this()));
}

void Fetcher::on_write( beast::error_code error, std::size_t bytes_transferred)
{
    if(error)
        return printError(error, "write");

    // Waiting for response
    http::async_read(stream_, buffer_, res_,beast::bind_front_handler( &Fetcher::on_read, shared_from_this()));
}

void Fetcher::on_read(beast::error_code error, std::size_t bytes_transferred)
{
    if(error)
        return printError(error, "read");

    std::cout << res_//.body()
    << std::endl;

//    http::async_write(stream_, req_,
//                      beast::bind_front_handler(
//                              &Fetcher::on_write,
//                              shared_from_this()));

    //To close connection and break the loop
    stream_.socket().shutdown(tcp::socket::shutdown_both, error);

}