#ifndef TCP_TEST_H
#define TCP_TEST_H

#include <algorithm>
#include <iterator>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>

#include "detail_tcp_acceptor.hpp"

namespace tcp_test {

static boost::barrier barrier(2);

void new_msg(detail::tcp_connection::pointer connection,const detail::raw_buffer_t &buffer, const boost::system::error_code &error)
{
    if(error)
        std::cout << "new_msg error\n";
    else
    {
        std::cout << "new_msg: ";
        std::copy(buffer.begin(),buffer.end(),std::ostream_iterator<detail::raw_buffer_t::value_type>(std::cout,""));
        std::cout << std::endl;
    }

    const std::string msg(buffer.begin(),buffer.end());

    if(msg != "quit")
        connection->async_read(boost::bind(new_msg,connection,_1,_2));
    else
        barrier.wait();
}

void new_one(detail::tcp_connection::pointer connection,const boost::system::error_code &error)
{
    if(error)
    {
        std::cout << "new_one error\n";
    }
    else
    {
        std::cout << "new_one geklappt\n";
        connection->async_read(boost::bind(new_msg,connection,_1,_2));
    }
}

void send_done(const boost::system::error_code &error)
{
    if(error)
        std::cout << "send_done error\n";
    else
        std::cout << "send_done geklappt\n";

}

}

BOOST_AUTO_TEST_CASE(tcp_test_case)
{
    using namespace tcp_test;

    boost::asio::io_service io_service;
    boost::scoped_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
    boost::thread t(boost::bind(&boost::asio::io_service::run,boost::ref(io_service)));

    detail::tcp_acceptor acceptor(io_service, 8585);
    acceptor.async_accept(new_one);

    detail::tcp_connection::pointer connection = detail::tcp_connection::connect(io_service,"localhost",8585);

    {
        std::string msg = "Hallo Welt";
        connection->async_write(msg,send_done);
    }

    {
        std::string msg = "es scheint";
        connection->async_write(msg,send_done);
    }

    boost::this_thread::sleep(boost::posix_time::seconds(1));

    {
        std::string msg = "wirklich zu funktionieren";
        connection->async_write(msg,send_done);
    }

    {
        std::string msg = "auch wenn man eine ganz lange Nachricht versendet und keine Ahnung hat ob das klappt";
        connection->async_write(msg,send_done);
    }

    boost::this_thread::sleep(boost::posix_time::seconds(2));

    {
        std::string msg = "quit";
        connection->async_write(msg,send_done);
    }

    barrier.wait();

    std::cout << "closing\n";

    connection->close();
    acceptor.close();

    std::cout << "closed\n";

    work.reset();
    t.join();
}

#endif // TCP_TEST_H
