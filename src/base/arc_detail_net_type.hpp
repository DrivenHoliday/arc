#ifndef ARC_DETAIL_NET_TYPE_HPP
#define ARC_DETAIL_NET_TYPE_HPP

#include <string>

#include <boost/serialization/string.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/variant.hpp>

namespace arc {
namespace detail {

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

typedef boost::variant<text_msg> net_type;

} }

#endif // ARC_DETAIL_NET_TYPE_HPP
