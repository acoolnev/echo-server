/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <boost/asio.hpp>

#include <iostream>
#include <memory>
#include <unordered_map>

namespace asio = boost::asio;

class Client
{
public:
    Client(boost::asio::io_service& ioService, const std::string& host, const std::string&  port) :
        m_ioService(ioService), m_socket(ioService)
    {
        boost::asio::ip::tcp::resolver resolver(m_ioService);
        auto endpoint = resolver.resolve(asio::ip::tcp::resolver::query(host, port));
        asio::connect(m_socket, endpoint);
    }
    
    void send(const std::string& message)
    {
        m_socket.send(asio::buffer(message));
    }
    

private:
    boost::asio::io_service& m_ioService;
    boost::asio::ip::tcp::socket m_socket;
};

class Server
{
public:
    Server(asio::io_service& ioService, int port) :
        m_ioService(ioService),
        m_acceptor(ioService, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {
        asyncAccept();
    }

private:
    class Session;
    using SessionPtr = std::shared_ptr<Session>;
    
    void asyncAccept()
    {
        SessionPtr session = std::make_shared<Session>(*this, m_ioService);
        m_acceptor.async_accept(session->socket(), [this, session](boost::system::error_code ec)
        {
            if (!ec)
            {
                m_sessions[&session->socket()] = std::move(session);
                std::cout << "Client session created" << std::endl;
                asyncAccept();
            }
            else
            {
                std::cout << "Error on accepting client session: " << ec.message() << std::endl;
            }
        });
    }
    
    void onSessionClose(SessionPtr session)
    {
        m_sessions.erase(&session->socket());
    }
    
    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        Session(Server& server, asio::io_service& ioService) :
            m_server(server), m_socket(ioService)
        {
        }
        
        asio::ip::tcp::socket& socket()
        {
            return m_socket;
        }
        
        void write(std::string message)
        {
            asio::async_write(m_socket, asio::buffer(message),
                [session = shared_from_this()](boost::system::error_code ec, std::size_t bytesWritten)
            {
                
            });   
        }
        
    private:
        void onRead(std::string&& message)
        {
            std::cout << "Client session received message: " << message << std::endl;
            
            // Write back to the client
            write(std::move(message));
        }
        
        void onClose()
        {
            m_server.onSessionClose(shared_from_this());   
        }
        
        void asyncRead()
        {
            async_read_until(m_socket, m_streambuf, "\n",
                [session = shared_from_this()](boost::system::error_code ec, std::size_t readBytes)
            {
                if (!ec)
                {
                    std::istream istream(&session->m_streambuf);
                    std::string message;
                    std::getline(istream, message);
                
                    session->onRead(std::move(message));
                }
                else if (ec == asio::error::eof)
                {
                    std::cout << "Client session is closed" << std::endl;
                    session->onClose();
                    return;
                }
                else
                {
                    std::cout << "Client session read error: " << ec.message() << std::endl;
                    return;
                }
                session->asyncRead();
            });
        }
        
        Server& m_server;
        asio::ip::tcp::socket m_socket;
        asio::streambuf m_streambuf;
    };
    
    
    boost::asio::io_service& m_ioService;
    boost::asio::ip::tcp::acceptor m_acceptor;
    
    using Sessions = std::unordered_map<asio::ip::tcp::socket*, SessionPtr>;
    Sessions m_sessions;
};

int main()
{

    return 0;
}
