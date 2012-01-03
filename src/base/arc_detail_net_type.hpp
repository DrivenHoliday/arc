#ifndef ARC_DETAIL_NET_TYPE_HPP
#define ARC_DETAIL_NET_TYPE_HPP

#include <cstring>

#include <set>
#include <string>

#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/variant.hpp>

namespace arc {
namespace msg {

struct text_msg
{
    text_msg()
    { }

    text_msg(const std::string &text)
        : text(text)
    { }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & text;
    }

    std::string text;
};

struct login_request
{
    login_request()
    { }

    login_request(const std::string &nickname)
        : nickname(nickname)
    { }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & nickname;
    }

    std::string nickname;
};

struct login_response
{
    login_response()
    { }

    login_response(const boost::uuids::uuid &uid)
        : uid(uid)
    { }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & uid;
    }

    boost::uuids::uuid uid;
};

struct nick_list
{
    struct nick
    {
        nick() {}

        nick(const boost::uuids::uuid &uid, const std::string &nickname)
            : uid(uid), nickname(nickname)
        { }

        boost::uuids::uuid uid;
        std::string nickname;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & uid;
            ar & nickname;
        }

        bool operator<(const nick &other) const
        {
            return uid < other.uid;
        }
    };

    typedef std::set<nick> list_t;

    nick_list()
    { }

    nick_list(const list_t &list)
        : list(list)
    { }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & list;
    }

    list_t list;
};

struct info_request
{
    info_request()
    { }

    template<class Archive>
    void serialize(Archive &, const unsigned int)
    {
    }
};

struct info_response
{
    info_response()
    { }

    info_response(std::size_t num_clients)
        : num_clients(num_clients)
    { }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & num_clients;
    }

    std::size_t num_clients;
};

}

namespace detail {

typedef boost::variant< msg::login_request
                      , msg::text_msg
                      , msg::login_response
                      , msg::nick_list
                      , msg::info_request
                      , msg::info_response > net_type;

} }

#endif // ARC_DETAIL_NET_TYPE_HPP
