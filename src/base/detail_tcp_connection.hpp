#ifndef DETAIL_TCP_CONNECTION_HPP
#define DETAIL_TCP_CONNECTION_HPP

#include <cassert>
#include <cstring>

#include <algorithm>
#include <limits>
#include <string>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/integer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "detail_raw_buffer.hpp"

namespace detail {

class tcp_acceptor;

class tcp_connection
        : public boost::enable_shared_from_this<tcp_connection>
{
public:
    BOOST_STATIC_CONSTANT(std::size_t, header_size = 2);
    BOOST_STATIC_CONSTANT(std::size_t, header_bit_size = header_size*8);

    typedef boost::uint_t<header_bit_size>::exact header_int_type;
    typedef boost::array<boost::uint8_t,header_size> header_arr_type;
    typedef boost::shared_ptr<header_arr_type> header_ptr;

    typedef boost::shared_ptr<tcp_connection> pointer;

    typedef boost::function<void(boost::system::error_code)> write_handler;
    typedef boost::function<void(raw_buffer_t, boost::system::error_code)> read_handler;

    typedef boost::uint16_t port_t;

    static pointer connect(boost::asio::io_service& io_service, const std::string &to, port_t port)
    {
        using namespace boost::asio::ip;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(to, boost::lexical_cast<std::string>(port));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        pointer result(new tcp_connection(io_service));

        boost::asio::connect(result->socket(), endpoint_iterator);

        return result;
    }

    boost::asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }

    template<typename Range>
    void async_write(const Range &range, write_handler handler)
    {
        assert(boost::size(range) < std::numeric_limits<header_int_type>::max());

        header_int_type header = boost::size(range);

        raw_buffer_t buffer(header_size + header);

        {
            typedef header_arr_type::value_type byte_type;
            const byte_type * const p = reinterpret_cast<byte_type*>(&header);
            std::copy(p,p+sizeof(header),buffer.begin());
        }

        boost::range::copy(range,buffer.begin()+header_size);

        boost::asio::async_write(m_socket, boost::asio::buffer(buffer),
                                 boost::bind(handler, boost::asio::placeholders::error));
    }

    void async_read(read_handler handler)
    {
        header_ptr header(new header_arr_type);
        boost::asio::async_read(m_socket,boost::asio::buffer(*header),
                                boost::bind(&tcp_connection::handle_header_read,shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred,
                                            handler,
                                            header));
    }

    void close()
    {
        m_socket.close();
    }

    friend class tcp_acceptor;

private:
    static pointer create(boost::asio::io_service& io_service)
    {
        return pointer(new tcp_connection(io_service));
    }

    tcp_connection(boost::asio::io_service& io_service)
        : m_socket(io_service)
    {
    }

    void handle_header_read(const boost::system::error_code& error,
                     size_t /*bytes_transferred*/,
                     read_handler handler,
                     header_ptr header)
    {
        if(error)
        {
            handler(raw_buffer_t(),error);
        }
        else
        {
            header_int_type header_int;
            typedef header_arr_type::value_type byte_type;
            boost::range::copy(*header,reinterpret_cast<byte_type*>(&header_int));

            boost::shared_ptr<raw_buffer_t> buffer(new raw_buffer_t(header_int));
            boost::asio::async_read(m_socket,boost::asio::buffer(*buffer),
                                    boost::bind(&tcp_connection::handle_read,shared_from_this(),
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred,
                                                handler,
                                                buffer));
        }
    }

    void handle_read(const boost::system::error_code& error,
                     size_t /*bytes_transferred*/,
                     read_handler handler,
                     boost::shared_ptr<raw_buffer_t> buffer)
    {
        if(error)
        {
            handler(raw_buffer_t(),error);
        }
        else
        {
            handler(*buffer,error);
        }
    }

    boost::asio::ip::tcp::socket m_socket;
};

}

#endif // TCP_CONNECTION_HPP
