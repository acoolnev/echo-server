#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <ostream>


namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Client
{
public:
   Client(asio::io_context& ioContext, const std::string& host, int port);
   
   void send(const std::string& message);
   std::string receive();

private:
   std::ostream& logPrefix() const;

   asio::io_context& m_ioContext;
   tcp::socket m_socket;
   asio::streambuf m_streambuf;
};
