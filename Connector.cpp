#include "Connector.h"

Connector::Connector(net::io_context &ioc, ssl::context &ctx) : resolver(ioc), stream(ioc, ctx) {
}

awaitable<void> Connector::establish_connection(char const* host, char const* port, char const* target, int version)
{
    beast::error_code ec;
    if(! SSL_set_tlsext_host_name(stream.native_handle(), host))
    {
        ec.assign(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
        std::cerr <<ec.message() << "\n";
        //return;
    }
    req.version(version);
    req.method(http::verb::get);
    req.target(target);
    req.set(http::field::host, host);
    req.keep_alive(true);
    req.set(boost::beast::http::field::connection, "keep-alive");

    auto results = co_await resolver.async_resolve(host, port, use_awaitable);

    beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));

    co_await beast::get_lowest_layer(stream).async_connect(results, use_awaitable);
    co_await stream.async_handshake(ssl::stream_base::client, use_awaitable);

    beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));

    run_queries();
}

void Connector::run_queries()
{
    auto io_context = stream.get_executor();
    co_spawn(io_context, Connector::perform_scan(), boost::asio::detached);

}

awaitable<void> Connector::perform_scan() {

    for(;;){
        beast::get_lowest_layer(stream).expires_after(std::chrono::seconds(30));
        http::response<http::string_body> current_res;

        co_await http::async_write(stream, req, use_awaitable);

        co_await http::async_read(stream, buffer, res, use_awaitable);

        write_out();
     }

}

void Connector::write_out()
{
    std::cout << res.body()<< std::endl;
    std::this_thread::sleep_for (std::chrono::seconds(2));
}
