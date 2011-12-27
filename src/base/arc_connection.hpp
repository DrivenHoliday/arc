#ifndef ARC_CONNECTION_HPP
#define ARC_CONNECTION_HPP

#include <boost/range.hpp>
#include <boost/range/algorithm/transform.hpp>

#include "arc_detail_net_type.hpp"
#include "detail_generic_connection.hpp"
#include "detail_generic_serialization.hpp"
#include "detail_raw_buffer.hpp"
#include "detail_tcp_acceptor.hpp"
#include "detail_tcp_connection.hpp"

namespace arc {

namespace detail {

struct encrypted_trait
{
    typedef ::detail::raw_buffer_t net_t;
    typedef ::detail::tcp_connection base_socket_t;
    typedef ::detail::tcp_acceptor base_acceptor_t;

    static net_t::value_type crypt(net_t::value_type t)
    {
        return t ^ 0x42;
    }

    static base_socket_t::net_t crypt_stream(const net_t &t)
    {
        ::detail::raw_buffer_t result(t.size());
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

typedef ::detail::generic_connection<encrypted_trait> encrypted_connection;
typedef ::detail::generic_acceptor<encrypted_trait> encrypted_acceptor;

struct arc_trait
{
    typedef arc::detail::net_type net_t;
    typedef encrypted_connection base_socket_t;
    typedef encrypted_acceptor base_acceptor_t;

    static base_socket_t::net_t write(const net_t &t)
    {
        return ::detail::serialize(t);
    }

    static net_t read(const base_socket_t::net_t &t)
    {
        return ::detail::deserialize<net_t>(t);
    }
};

}

typedef ::detail::generic_connection<detail::arc_trait> connection;
typedef ::detail::generic_acceptor<detail::arc_trait> acceptor;

}

#endif // ARC_CONNECTION_HPP
