#ifndef SERIALIZATION_TEST_HPP
#define SERIALIZATION_TEST_HPP

#include <string>

#include "detail_generic_serialization.hpp"

BOOST_AUTO_TEST_CASE(tcp_test_case)
{
    int i = 42;

    detail::raw_buffer_t buf = detail::serialize(i);

    BOOST_CHECK_EQUAL(detail::deserialize<int>(buf),i);
}

#endif // SERIALIZATION_TEST_HPP
