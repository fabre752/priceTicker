
#include "Connector.h"

Connector::Connector(net::io_context &ioc) : resolver(ioc), stream(ioc){

}

void Connector::establish_connection(char const* host, char const* port, char const* target, int version) {
    req.version(version);
    req.method(http::verb::get);
    req.target(target);
    req.set(http::field::host, host);
    req.keep_alive(true);
    req.set(boost::beast::http::field::connection, "keep-alive");

    auto const results = resolver.resolve(host, port);
    stream.connect(results);

    run_queries();
}

void Connector::run_queries()
{
    auto generator = perform_scan();

    auto scan_handle = perform_scan().coro;
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
    std::cout << res//.body()
    << std::endl;
}
