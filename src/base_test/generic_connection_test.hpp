#ifndef GENERIC_CONNECTION_TEST_HPP
#define GENERIC_CONNECTION_TEST_HPP

#include <algorithm>
#include <iterator>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/serialization/access.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>

#include "detail_tcp_acceptor.hpp"
#include "detail_tcp_connection.hpp"
#include "detail_generic_connection.hpp"
#include "detail_generic_serialization.hpp"

namespace generic_connection_test {

class gps_position
{
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & degrees;
        ar & minutes;
        ar & seconds;
    }

    int degrees;
    int minutes;
    float seconds;

    friend std::ostream& operator<<(std::ostream &ostream, const gps_position &pos)
    {
        ostream << "gps_position(" << pos.degrees << ", " << pos.minutes << ", " << pos.seconds << ")";
        return ostream;
    }

public:
    gps_position(){};
    gps_position(int d, int m, float s) :
        degrees(d), minutes(m), seconds(s)
    {}

    bool valid() const
    {
        return minutes != 0;
    }
};

struct encrypted_trait
{
    typedef detail::raw_buffer_t net_t;
    typedef detail::tcp_connection base_socket_t;
    typedef detail::tcp_acceptor base_acceptor_t;

    static net_t::value_type crypt(net_t::value_type t)
    {
        return t ^ 0x42;
    }

    static base_socket_t::net_t crypt_stream(const net_t &t)
    {
        raw_buffer_t result(t.size());
        boost::range::transform(t,result.begin(),crypt);
        return result;
    }

    static base_socket_t::net_t write(const net_t &t)
    {
        return crypt_stream(t);
    }

    static net_t read(const base_socket_t::net_t &t)
    {
        return crypt_stream(t);
    }
};

typedef detail::generic_connection<encrypted_trait> encrypted_connection_t;
typedef detail::generic_acceptor<encrypted_trait> encrypted_acceptor_t;

struct trait
{
    typedef gps_position net_t;
    typedef encrypted_connection_t base_socket_t;
    typedef encrypted_acceptor_t base_acceptor_t;

    static base_socket_t::net_t write(const net_t &t)
    {
        return detail::serialize(t);
    }

    static net_t read(const base_socket_t::net_t &t)
    {
        return detail::deserialize<net_t>(t);
    }
};

typedef detail::generic_connection<trait> connection_t;
typedef detail::generic_acceptor<trait> acceptor_t;

static boost::barrier barrier(2);

void new_msg(connection_t::pointer connection,const gps_position &buffer, const boost::system::error_code &error)
{
    if(error)
        std::cout << "new_msg error\n";
    else
    {
        std::cout << "new_msg: " << buffer << std::endl;
    }

    if(buffer.valid())
        connection->async_read(boost::bind(new_msg,connection,_1,_2));
    else
        barrier.wait();
}

void new_one(connection_t::pointer connection,const boost::system::error_code &error)
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

BOOST_AUTO_TEST_CASE(generic_connection_test_case)
{
    using namespace generic_connection_test;

    boost::asio::io_service io_service;
    boost::scoped_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
    boost::thread t(boost::bind(&boost::asio::io_service::run,boost::ref(io_service)));

    acceptor_t acceptor(io_service, 8585);
    acceptor.async_accept(new_one);

    connection_t::pointer connection = connection_t::connect(io_service,"localhost",8585);

    {
        gps_position msg(10,20,4.2f);
        connection->async_write(msg,send_done);
    }

    {
        gps_position msg(45,34,55.2f);
        connection->async_write(msg,send_done);
    }

    boost::this_thread::sleep(boost::posix_time::seconds(1));

    {
        gps_position msg(345,5,42.45f);
        connection->async_write(msg,send_done);
    }

    boost::this_thread::sleep(boost::posix_time::seconds(2));

    {
        gps_position msg(0,0,0.f);
        connection->async_write(msg,send_done);
    }

    barrier.wait();

    std::cout << "closing\n";

    connection->close();
    acceptor.close();

    std::cout << "closed\n";

    work.reset();
    t.join();
}}


#endif // GENERIC_CONNECTION_TEST_HPP
