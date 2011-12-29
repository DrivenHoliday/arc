#ifndef ARC_SERVER_HPP
#define ARC_SERVER_HPP

#include <cstring>

#include <algorithm>
#include <iterator>
#include <set>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/function.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/noncopyable.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "arc_connection.hpp"
#include "utility.hpp"

namespace arc {

class server : boost::noncopyable
{
public:
    BOOST_STATIC_CONSTANT(std::size_t num_worker, = 10);

    typedef arc::connection::port_t port_t;

    server(port_t port)
        : m_work(new boost::asio::io_service::work(m_io_service))
        , m_acceptor(m_io_service, port)
    {
        for(std::size_t n = 0; n < num_worker; ++n)
        {
            m_worker.add_thread
                    ( new boost::thread
                      ( utility::try_catch<void()>
                           ( boost::bind
                                ( &boost::asio::io_service::run
                                , boost::ref(m_io_service) ) ) ) );
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
    struct connection_t
    {
        boost::uuids::uuid uid;
        std::string nickname;
        arc::connection::pointer connection;

        bool operator<(const connection_t &other) const
        {
            return uid < other.uid;
        }
    };

    typedef std::set<connection_t> connection_set;

    void accept()
    {
        m_acceptor.async_accept
                ( boost::bind
                    ( &server::new_connetion
                    , this
                    , _1
                    , _2 ) );
    }

    void new_connetion(arc::connection::pointer connection, const boost::system::error_code &error)
    {
        if(!error)
        {
            connection->async_read
                    ( boost::bind
                        ( &server::handle_new_connection_login
                        , this
                        , connection
                        , _1
                        , _2 ) );
        }
        accept();
    }

    void handle_new_connection_login(arc::connection::pointer connection, arc::detail::net_type t, const boost::system::error_code &error)
    {
        connection_set::value_type conn;
        conn.uid = m_generate_random_uid();
        conn.nickname = boost::get<arc::msg::login_request>(t).nickname;
        conn.connection = connection;

        connection->async_write
                ( arc::msg::login_response(conn.uid)
                , boost::bind
                    ( &server::new_connetion_response_written
                    , this
                    , conn
                    , _1 ) );
    }

    arc::msg::nick_list::list_t generate_nick_list()
    {
        arc::msg::nick_list::list_t result;

        namespace l=boost::lambda;

        boost::range::transform
                    ( m_connections
                    , std::inserter(result,result.begin())
                    , l::bind
                        ( l::constructor<arc::msg::nick_list::nick>()
                        , l::bind(&connection_set::value_type::uid,l::_1)
                        , l::bind(&connection_set::value_type::nickname,l::_1) ) );

        return result;
    }

    void new_connetion_response_written(connection_set::value_type connection, const boost::system::error_code &error)
    {
        m_connections.insert(connection);

        connection.connection->async_read
                ( boost::bind
                    ( &server::new_msg
                    , this,connection
                    ,_1
                    ,_2 ) );

        connection.connection->async_write
                ( arc::msg::nick_list(generate_nick_list())
                , boost::bind
                    ( &server::msg_written
                    , this
                    , connection
                    , _1 ) );
    }

    void msg_written(connection_set::value_type sender, const boost::system::error_code &error)
    {

    }

    void new_msg(connection_set::value_type receiver, arc::detail::net_type t, const boost::system::error_code &error)
    {
        receiver.connection->async_read
                ( boost::bind
                        ( &server::new_msg
                        , this
                        , receiver
                        , _1
                        , _2 ) );
    }

    boost::asio::io_service m_io_service;
    boost::scoped_ptr<boost::asio::io_service::work> m_work;
    boost::thread_group m_worker;
    arc::acceptor m_acceptor;
    boost::uuids::random_generator m_generate_random_uid;
    connection_set m_connections;
};

}

#endif // ARC_SERVER_HPP
