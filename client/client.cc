#include "client.h"

#include <exception>
#include <iostream>

const char* MESSAGE_DELIMITER = "\n";

Client::Client(asio::io_context& ioContext, const std::string& host, int port) :
   m_ioContext(ioContext), m_socket(ioContext)
{
   try
   {
      tcp::resolver resolver(m_ioContext);
      auto endpoint = resolver.resolve(tcp::resolver::query(host, std::to_string(port)));
      asio::connect(m_socket, endpoint);
   }
   catch (const boost::system::system_error& error)
   {
      throw std::runtime_error(error.what());
   }
}

void Client::send(const std::string& message)
{
   try
   {
      asio::write(m_socket, asio::buffer(message + MESSAGE_DELIMITER));
   }
   catch (const boost::system::system_error& error)
   {
      throw std::runtime_error(error.what());
   }
}

std::string Client::receive()
{
   try
   {
      asio::read_until(m_socket, m_streambuf, MESSAGE_DELIMITER);
   }
   catch (const boost::system::system_error& error)
   {
      if (error.code() == asio::error::eof)
         throw std::runtime_error("Server closed connection");
      else
         throw std::runtime_error(error.what());
   }

   std::istream istream(&m_streambuf);
   std::string message;
   std::getline(istream, message);
   return message;
}

std::ostream &Client::logPrefix() const
{
   return std::cout << "Client: ";
}
