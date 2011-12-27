#ifndef DETAIL_GENERIC_CONNECTION_HPP
#define DETAIL_GENERIC_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "detail_tcp_acceptor.hpp"
#include "detail_tcp_connection.hpp"

namespace detail {

template<typename Traits>
class generic_acceptor;

template<typename Traits>
class generic_connection
        : public boost::enable_shared_from_this< generic_connection<Traits> >
{
public:
    typedef boost::shared_ptr< generic_connection<Traits> > pointer;

    typedef typename Traits::base_socket_t base_socket_t;
    typedef typename Traits::net_t net_t;

    typedef boost::function<void(boost::system::error_code)> write_handler;
    typedef boost::function<void(net_t, boost::system::error_code)> read_handler;

    typedef tcp_connection::port_t port_t;

    static pointer connect(boost::asio::io_service& io_service, const std::string &to, port_t port)
    {
        typename base_socket_t::pointer base = base_socket_t::connect(io_service,to,port);

        return generic_connection::create(io_service, base);
    }

    base_socket_t& socket()
    {
        return m_socket;
    }

    void async_write(const net_t &t, write_handler handler)
    {
        m_socket->async_write(Traits::write(t),handler);
    }

    void async_read(read_handler handler)
    {
        m_socket->async_read(boost::bind(&generic_connection<Traits>::handle_read,this->shared_from_this(),_1,_2,handler));
    }

    void close()
    {
        m_socket->close();
    }

    friend class generic_acceptor<Traits>;

private:
    static pointer create(boost::asio::io_service& io_service, typename base_socket_t::pointer socket)
    {
        pointer result(new generic_connection<Traits>(io_service,socket));
        return result;
    }

    generic_connection(boost::asio::io_service&, typename base_socket_t::pointer socket)
        : m_socket(socket)
    {

    }

    void handle_read(const typename base_socket_t::net_t &t, const boost::system::error_code &error, read_handler handler)
    {
        if(!error)
            handler(Traits::read(t),error);
        else
            handler(net_t(),error);
    }

    typename base_socket_t::pointer m_socket;
};

template<typename Traits>
class generic_acceptor
{
public:
    typedef generic_connection<Traits> connection_t;
    typedef boost::function<void(typename connection_t::pointer, boost::system::error_code)> accept_handler;

    generic_acceptor(boost::asio::io_service& io_service, typename Traits::base_socket_t::port_t port)
        : m_acceptor(io_service, port)
    {

    }

    void async_accept(accept_handler handler)
    {
        m_acceptor.async_accept(boost::bind(&generic_acceptor::handle_accept, this, _1,
                                           _2,handler));
    }

    void close()
    {
        m_acceptor.close();
    }

    boost::asio::io_service& get_io_service()
    {
        return m_acceptor.get_io_service();
    }

private:
    void handle_accept(typename connection_t::base_socket_t::pointer new_connection,
                       const boost::system::error_code& error,
                       accept_handler handler)
    {
        if (error)
        {
           handler(typename connection_t::pointer(),error);
        }
        else
        {
            handler(connection_t::create(m_acceptor.get_io_service(),new_connection),error);
        }
    }

    typename Traits::base_acceptor_t m_acceptor;
};

#endif // DETAIL_GENERIC_CONNECTION_HPP
