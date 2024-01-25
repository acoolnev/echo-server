#include "session.h"
#include "server.h"

#include <iostream>
#include <string>

const std::size_t READ_BUFFER_SIZE = 8 * 1024;

namespace
{

std::ostream& logData(const void* data, std::size_t size)
{
   for (const char symbol: std::string_view(reinterpret_cast<const char*>(data), size))
   {
      if (std::isalnum(symbol) || std::ispunct(symbol))
         std::cout << symbol;
      else
         std::cout << ' ';
   }

   return std::cout;
}

}

Session::Session(Server& server, asio::io_context& ioContext) :
   m_server(server), m_socket(ioContext), m_buffer(READ_BUFFER_SIZE)
{
}

Session::~Session()
{
   close();
}

void Session::start()
{
   m_clientAddress = m_socket.remote_endpoint().address().to_string();
   m_clientAddress += ":";
   m_clientAddress += std::to_string(m_socket.remote_endpoint().port());

   logPrefix() << "session created" << std::endl;

   asyncRead();
}

void Session::close()
{
   m_socket.close();
}

void Session::write(const void* data, std::size_t size)
{
   asio::async_write(m_socket, asio::buffer(data, size),
      [this](boost::system::error_code ec, std::size_t bytesWritten)
   {
      if (ec)
         logPrefix() << "write error: " << ec.message() << std::endl;
   });   
}

void Session::onRead(const void* data, std::size_t size)
{
   logPrefix() << "received message: ";
   logData(data, size) << std::endl;
    
   // Send echo to the client
   write(data, size);
}

void Session::onClose()
{
   m_server.onSessionClose(shared_from_this());
}

void Session::asyncRead()
{
   asio::async_read(m_socket, asio::buffer(m_buffer.data(), m_buffer.size()),
      boost::asio::transfer_at_least(1),
      [this](boost::system::error_code ec, std::size_t readBytes)
   {
      if (!ec)
      {
         onRead(m_buffer.data(), readBytes);
         asyncRead();
      }
      else
      {
         if (ec == asio::error::eof) 
            logPrefix() << "session closed due to disconnected client" << std::endl;
         else if (ec == asio::error::operation_aborted)
            logPrefix() << "session closed due to socket closure" << std::endl;
         else   
            logPrefix() << "read error: " << ec.message() << std::endl;

         onClose();
         return;
      }
   });
}

std::ostream& Session::logPrefix() const
{
   return std::cout << "Session [" << m_clientAddress << "]: ";
}
