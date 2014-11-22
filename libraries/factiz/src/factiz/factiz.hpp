#ifndef _FACTIZ_HPP_
#define	_FACTIZ_HPP_

#include <boost/multiprecision/cpp_int.hpp> 

namespace boost
{
namespace multiprecision
{

inline cpp_int abs( const cpp_int & x )
{ return x * x.sign(); }

}
}

namespace factiz
{
using boost::multiprecision::abs;
typedef boost::multiprecision::cpp_int int_type;

/**
 * @brief factorize the product of two bigs prime numbers
 * @param pq p*q input number
 * @param p output p
 * @param q output q
 * @return true if success
 */
bool factorize( const int_type & pq, int_type & p, int_type & q );
    
}


#endif
