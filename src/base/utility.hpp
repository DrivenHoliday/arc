#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <exception>
#include <iostream>

#include <boost/function.hpp>
#include <boost/function_types/result_type.hpp>

namespace utility {

template<typename Func>
struct try_catch_t
{
    try_catch_t(boost::function<Func> f)
        : f(f)
    { }

    typename boost::function_types::result_type<Func>::type operator()() const
    {
        try
        {
            f();
        }
        catch(std::exception &e)
        {
            std::cout << "error: " << typeid(e).name() << ": " << e.what() << std::endl;
        }
        catch(...)
        {
            std::cout << "error: unknown error\n";
        }
    }

private:
    const boost::function<Func> f;
};

template<typename Func>
try_catch_t<Func> try_catch(boost::function<Func> f)
{
    return try_catch_t<Func>(boost::function<Func>(f));
}

}

#endif // UTILITY_HPP
