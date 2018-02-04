
#ifndef NONDET_INCLUDED
#define NONDET_INCLUDED 1

#include <iostream>
#include <functional>
#include <type_traits>


struct failure { };

inline std::ostream& operator << ( std::ostream& out, failure f )
{
   std::cout << "computation failed\n";
   return out;
}

inline void require( bool b )
{
   if(!b) throw failure( );
}


struct none_matched : std::runtime_error
{
    none_matched(const std::string& str ):
            runtime_error(str)
    {}
};




template <typename FailureException = failure, typename ACallable>
typename std::result_of<ACallable()>::type try_in_sequence(ACallable&& callable)
{
    try {
        return callable();
    }
    catch (FailureException)
    {
        throw none_matched{"the argument didn't match any of the patterns"};
    }
}

template <typename FailureException = failure, typename ACallable, typename ... Callables>
typename std::result_of<ACallable()>::type try_in_sequence(ACallable&& acallable,
                                                           Callables&& ... callables)
{
    try {
        return acallable();
    }
    catch (FailureException)
    {
        return try_in_sequence<FailureException>(std::forward<Callables>(callables)...);
    }
}

#endif 

