
#include "Connector.h"

Connector::Connector(net::io_context &ioc, ssl::context &ctx) : resolver(ioc), stream(ioc, ctx){

}

void Connector::establish_connection(char const* host, char const* port, char const* target, int version)
{
    beast::error_code ec;
    if(! SSL_set_tlsext_host_name(stream.native_handle(), host))
    {
        ec.assign(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
        std::cerr <<ec.message() << "\n";
        return;
    }
    req.version(version);
    req.method(http::verb::get);
    req.target(target);
    req.set(http::field::host, host);
    req.keep_alive(true);
    req.set(boost::beast::http::field::connection, "keep-alive");

    auto const results = resolver.resolve(host, port);
    beast::get_lowest_layer(stream).connect(results);
    stream.handshake(ssl::stream_base::client);
    run_queries();
}

void Connector::run_queries()
{
    auto generator = perform_scan();
    auto scan_handle = generator.coro;
    auto &promise = scan_handle.promise();
    while(true)
    {
        scan_handle.resume();

        std::this_thread::sleep_for(std::chrono::seconds(5));

        res = promise.current_value;
        write_out();
    }
}

Generator Connector::perform_scan() {

    while(true){

        beast::flat_buffer buffer;
        http::response<http::string_body> current_res;

        http::write(stream, req);
        http::read(stream, buffer, current_res);

        co_yield current_res;
    }
}

void Connector::write_out()
{
    std::cout << res.body()
    << std::endl;
}
