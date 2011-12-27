#ifndef DETAIL_GENERIC_SERIALIZATION_HPP
#define DETAIL_GENERIC_SERIALIZATION_HPP

#include <string>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "detail_raw_buffer.hpp"

namespace detail {

template<typename T>
raw_buffer_t serialize(const T &t)
{
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << t;
    const ::std::string &str = ss.str();
    raw_buffer_t result(str.size());
    boost::range::copy(str,result.begin());
    return result;
}

template<typename T>
T deserialize(const raw_buffer_t &buffer)
{
    const ::std::string str(buffer.begin(),buffer.end());
    std::stringstream ss(str);
    boost::archive::text_iarchive ia(ss);
    T t;
    ia >> t;
    return t;
}

}

#endif // DETAIL_GENERIC_SERIALIZATION_H
