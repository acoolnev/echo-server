#include "server.h"

#include <boost/asio.hpp>

const int PORT = 8181;

namespace asio = boost::asio;

int main()
{
    asio::io_context ioContext;
    asio::io_context::work work(ioContext);

    Server server(ioContext, PORT);

    ioContext.run();

    return 0;
}
