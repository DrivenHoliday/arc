#include <iostream>
#include <iterator>
#include <algorithm>

#include <boost/bind.hpp>

#include "detail_tcp_acceptor.hpp"

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

    connection->async_read(boost::bind(new_msg,connection,_1,_2));
}

void new_one(detail::tcp_connection::pointer connection,const boost::system::error_code &error)
{
    if(error)
        std::cout << "new_one error\n";
    else
        std::cout << "new_one geklappt\n";

    connection->async_read(boost::bind(new_msg,connection,_1,_2));
}

void send_done(const boost::system::error_code &error)
{
    if(error)
        std::cout << "send_done error\n";
    else
        std::cout << "send_done geklappt\n";

}

int main(int argc, char *argv[])
{
    boost::asio::io_service io_service;

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

    {
        std::string msg = "wirklich zu funktionieren";
        connection->async_write(msg,send_done);
    }

    {
        std::string msg = "auch wenn man eine ganz lange Nachricht versendet und keine Ahnung hat ob das klappt";
        connection->async_write(msg,send_done);
    }

    io_service.run();
}
