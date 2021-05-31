#include <memory>
#include "Connector.h"

int main() {
    auto const host = "finnhub.io";
    auto const port = "443";
    auto const target = "/api/v1/crypto/candle?symbol=BINANCE:BTCUSDT&resolution=D&from=1572651390&to=1575243390&token=c2n7c0iad3i8g7sqr60g";
    int version = 10;

    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
    ctx.set_default_verify_paths();

    auto ptr = std::make_unique<Connector>(ioc, ctx);

    boost::asio::co_spawn(ioc, [host, port, target, version, &ptr] {
        return ptr->establish_connection(std::move(host), std::move(port), std::move(target), std::move(version));
    }, boost::asio::detached);
    ioc.run();

    return 0;
}