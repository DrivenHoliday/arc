#ifndef ARC_TEST_HPP
#define ARC_TEST_HPP

#include "arc_server.hpp"
#include "arc_client.hpp"

BOOST_AUTO_TEST_CASE(arc_test_case)
{
    arc::server server(8585);

    arc::client::pointer a = arc::client::connect("localhost",8585,"florian");
    arc::client::pointer b = arc::client::connect("localhost",8585,"benny");

    std::cout << "closing\n";
}

#endif // ARC_TEST_HPP
