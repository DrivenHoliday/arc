#ifndef ARC_CLIENT_HPP
#define ARC_CLIENT_HPP

#include <cstring>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include "arc_connection.hpp"
#include "utility.hpp"

namespace arc {

class client
    : boost::noncopyable, public boost::enable_shared_from_this<client>
{
public:
    typedef boost::shared_ptr<client> pointer;
    typedef arc::connection::port_t port_t;
    typedef boost::function<void(pointer,boost::system::error_code)> connect_handler;

private:
    struct pending_connection : boost::noncopyable
    {
        typedef boost::shared_ptr<pending_connection> pointer;

        pending_connection(const std::string &nickname)
            : work(new boost::asio::io_service::work(io_service))
            , thread( utility::try_catch<void()>
                           ( boost::bind
                                ( &boost::asio::io_service::run
                                , boost::ref(io_service) ) ) )
            , nickname(nickname)
        { }

        ~pending_connection()
        {
            work.reset();
            thread.join();
        }

        boost::asio::io_service io_service;
        boost::scoped_ptr<boost::asio::io_service::work> work;
        boost::thread thread;
        arc::connection::pointer connection;
        std::string nickname;
        boost::uuids::uuid uid;
    };

    static void login_request_sent(connect_handler handler, pending_connection::pointer connection, const boost::system::error_code &error)
    {
        connection->connection->async_read
                ( boost::bind
                    ( client::login_response
                    , handler
                    , connection
                    , _1
                    , _2 ) );
    }

    static void login_response(connect_handler handler, pending_connection::pointer connection, arc::detail::net_type msg, const boost::system::error_code &error)
    {
        connection->uid = boost::get<arc::msg::login_response>(msg).uid;

        pointer p = pointer(new client(connection));

        p->start();

        handler
            ( p
            , error );
    }

    static void call_promise(boost::promise<pointer> &promise, pointer p, const boost::system::error_code &error)
    {
        promise.set_value(p);
    }

public:
    static void async_connect(const std::string &to, port_t port, const std::string &nickname, connect_handler handler)
    {
        pending_connection::pointer connection(new pending_connection(nickname));

        connection->connection = arc::connection::connect(connection->io_service,to,port);

        connection->connection->async_write
                ( arc::msg::login_request(nickname)
                , boost::bind
                        ( client::login_request_sent
                        , handler
                        , connection
                        , _1 ) );
    }

    static pointer connect(const std::string &to, port_t port, const std::string &nickname)
    {
        boost::promise<pointer> promise;

        async_connect
                ( to
                , port
                , nickname
                , boost::bind
                        ( client::call_promise
                        , boost::ref(promise)
                        , _1
                        , _2 ) );

        boost::unique_future<pointer> future = promise.get_future();

        pointer p = future.get();

        return p;
    }

    struct server_info
    {
        server_info(std::size_t num_clients)
            : num_clients(num_clients)
        { }

        std::size_t num_clients;
    };

    typedef boost::function<void(server_info, const boost::system::error_code &error)> server_info_handler;

    static void call_server_info_handler(server_info_handler handler, arc::detail::net_type msg, const boost::system::error_code &error)
    {
        handler(server_info(boost::get<arc::msg::info_response>(msg).num_clients),error);
    }

    static void server_info_written(arc::connection::pointer connection, server_info_handler handler, const boost::system::error_code &error)
    {
        if(error)
        {
            handler(server_info(0),error);
        }
        else
        {
            connection->async_read
                    ( boost::bind
                        ( client::call_server_info_handler
                        , handler
                        , _1
                        , _2 ) );
        }
    }

    static void get_server_info(boost::asio::io_service &io_service, const std::string &to, port_t port, server_info_handler handler)
    {
        arc::connection::pointer connection = arc::connection::connect(io_service,to,port);

        connection->async_write
                ( arc::msg::info_request()
                , boost::bind
                        ( client::server_info_written
                        , connection
                        , handler
                        , _1 ) );
    }

private:
    client(pending_connection::pointer connection)
        : m_connection(connection)
    { }

    void start()
    {
        m_connection->connection->async_read
                ( boost::bind
                        ( &client::new_msg
                        , shared_from_this()
                        , _1
                        , _2 ) );
    }

    void new_msg(const arc::detail::net_type &msg, const boost::system::error_code &error)
    {
        start();
    }

    pending_connection::pointer m_connection;
};

}

#endif // ARC_CLIENT_HPP
