#include "server.h"
#include "session.h"

#include <iostream>


Server::Server(asio::io_context& ioContext, int port) :
    m_ioContext(ioContext),
    m_acceptor(ioContext, tcp::endpoint(tcp::v4(), port))
{
   logPrefix() << "begin accepting client sessions on port " << port << std::endl;
   asyncAccept();
}

void Server::asyncAccept()
{
   SessionPtr session = std::make_shared<Session>(*this, m_ioContext);
   m_acceptor.async_accept(session->socket(), [this, session](boost::system::error_code ec)
   {
      if (!ec)
      {
         m_sessions.emplace(session);
         session->start();
         asyncAccept();
      }
      else
      {
         if (ec == asio::error::operation_aborted)
            logPrefix() << "server has been shut down" << std::endl;
         else
            logPrefix() << "error on accepting client session: " << ec.message() << std::endl;
            
         return;
      }
   });
}

std::ostream &Server::logPrefix() const
{
   return std::cout << "Server: ";
}

void Server::onSessionClose(SessionPtr session)
{
   m_sessions.erase(session);
}
