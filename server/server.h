#pragma once

#include <boost/asio.hpp>

#include <memory>
#include <unordered_set>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Session;
using SessionPtr = std::shared_ptr<Session>;

class Server
{
public:
    Server(asio::io_context& ioContext, int port);

    void onSessionClose(SessionPtr session);

private:
    void asyncAccept();

    std::ostream& logPrefix() const;
    
    asio::io_context& m_ioContext;
    tcp::acceptor m_acceptor;
    
    using Sessions = std::unordered_set<SessionPtr>;
    Sessions m_sessions;
};
