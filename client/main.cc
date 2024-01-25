#include "client.h"

#include <boost/asio.hpp>
#include <iostream>
#include <string>

const char* HOST = "127.0.0.1";
const int PORT = 8181;

namespace asio = boost::asio;

int main()
{
   asio::io_context ioContext;

   try
   {
      Client client(ioContext, HOST, PORT);

      while (true)
      {
         std::cout << "Input: ";
         std::string message;
         std::getline(std::cin, message);
         
         client.send(message);
         message = client.receive();

         std::cout << "Echo: " << message << std::endl;
      }
   }
   catch (std::exception& exception)
   {
      std::cout << "Error: " << exception.what() << std::endl;
   }

   return 0;
}
