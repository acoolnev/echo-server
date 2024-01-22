#include <iostream>
#include <memory>
#include <boost/asio.hpp>

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
    void asyncAccept()
    {
        SessionPtr session = std::make_shared<Session>(m_ioService);
        m_acceptor.async_accept(session->socket(), [this, session](boost::system::error_code ec)
        {
            if (!ec)
            {
                m_sessions.emplace_back(std::move(session));
                std::cout << "Client session created" << std::endl;
                asyncAccept();
            }
            else
            {
                std::cout << "Error on accepting client session: " << ec.message() << std::endl;
            }
        });
    }
    
    class Session
    {
    public:
        Session(asio::io_service& ioService) :
            m_socket(ioService)
        {
        }
        
        asio::ip::tcp::socket& socket()
        {
            return m_socket;
        }
        
    private:
        asio::ip::tcp::socket m_socket;
    };
    
    using SessionPtr = std::shared_ptr<Session>;
    
    boost::asio::io_service& m_ioService;
    boost::asio::ip::tcp::acceptor m_acceptor;
    
    using Sessions = std::vector<SessionPtr>;
    Sessions m_sessions;
};

int main()
{

    return 0;
}
