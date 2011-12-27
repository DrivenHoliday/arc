#ifndef ARC_SERVER_HPP
#define ARC_SERVER_HPP

#include <cstring>

#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include "arc_connection.hpp"

namespace arc {

class server : boost::noncopyable
{
public:
    BOOST_STATIC_CONSTANT(std::size_t num_worker, = 10);

    typedef arc::connection::port_t port_t;

    server(port_t port)
        : m_work(new boost::asio::io_service::work(m_io_service)), m_acceptor(m_io_service,port)
    {
        for(std::size_t n = 0; n < num_worker; ++n)
        {
            m_worker.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run,boost::ref(m_io_service))));
        }

        accept();
    }

    ~server()
    {
        m_acceptor.close();
        m_work.reset();
        m_worker.join_all();
    }

private:
    void accept()
    {
        m_acceptor.async_accept(boost::bind(&server::new_connetion,this,_1,_2));
    }

    void new_connetion(arc::connection::pointer connection, const boost::system::error_code &error)
    {
        if(!error)
            m_connections.push_back(connection);
        accept();
    }

    boost::asio::io_service m_io_service;
    boost::scoped_ptr<boost::asio::io_service::work> m_work;
    boost::thread_group m_worker;
    arc::acceptor m_acceptor;
    typedef std::vector<arc::connection::pointer> connection_vec;
    connection_vec m_connections;
};

}

#endif // ARC_SERVER_HPP
