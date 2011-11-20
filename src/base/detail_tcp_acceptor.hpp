#ifndef DETAIL_TCP_ACCEPTOR_HPP
#define DETAIL_TCP_ACCEPTOR_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "detail_tcp_connection.hpp"

namespace detail {

class tcp_acceptor
{
public:
    typedef boost::function<void(tcp_connection::pointer, boost::system::error_code )> accept_handler;

    tcp_acceptor(boost::asio::io_service& io_service, tcp_connection::port_t port)
        : m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {

    }

    void async_accept(accept_handler handler)
    {
        tcp_connection::pointer new_connection =
                tcp_connection::create(m_acceptor.get_io_service());

        m_acceptor.async_accept(new_connection->socket(),
                               boost::bind(&tcp_acceptor::handle_accept, this, new_connection,
                                           boost::asio::placeholders::error,handler));
    }

private:
    void handle_accept(tcp_connection::pointer new_connection,
                       const boost::system::error_code& error,
                       accept_handler handler)
    {
        if (error)
        {
            new_connection.reset();
        }

        handler(new_connection,error);
    }

    boost::asio::ip::tcp::acceptor m_acceptor;
};

}

#endif // TCP_SERVER_HPP
