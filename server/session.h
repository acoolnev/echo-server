#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <memory>
#include <ostream>

class Server;

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
   Session(Server& server, asio::io_context& ioContext);
   ~Session();
   
   void start();
   void close();

   tcp::socket& socket()
      { return m_socket; }
   
   void write(const void* data, std::size_t size);
    
private:
   void onRead(const void* data, std::size_t size);
   void onClose();
   
   void asyncRead();

   std::ostream& logPrefix() const;
   
   Server& m_server;
   tcp::socket m_socket;
   std::vector<std::uint8_t> m_buffer;

   std::string m_clientAddress;
};
