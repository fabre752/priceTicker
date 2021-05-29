#include <memory>

#include "Connector.h"

int main()
{
    auto const host = "finnhub.io";
    auto const port = "8080";
    auto const target = "/api/v1/crypto/candle?symbol=BINANCE:BTCUSDT&resolution=D&from=1572651390&to=1575243390&token=c2n7c0iad3i8g7sqr60g";
    int version = 11;

    boost::asio::io_context ioc;

    std::make_shared<Connector>(ioc)->establish_connection(host, port, target, version);

    return 0;
}