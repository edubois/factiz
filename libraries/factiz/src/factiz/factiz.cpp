#include "factiz.hpp"
#include <boost/multiprecision/cpp_int.hpp>

namespace factiz
{

using int_type = boost::multiprecision::cpp_int;

namespace
{

inline int_type abs_int(const int_type& x)
{
    return x < 0 ? -x : x;
}

inline int_type gcd_int(int_type a, int_type b)
{
    while (b != 0)
    {
        int_type t = a % b;
        a = b;
        b = t;
    }
    return a;
}

bool step_search(const int_type& k, int_type& x, int_type& y)
{
    if (x <= 1) return false;

    y = k / x;   // 🔥 invariant projection (critical fix)

    if (x * y == k)
        return true;

    return false;
}

} // namespace

bool factorize(const int_type& k, int_type& p, int_type& q)
{
    // start near sqrt(k)
    int_type x = sqrt(k);
    if (x * x < k) ++x;

    const int_type limit = 1;

    for (int i = 0; i < 200000000; ++i)
    {
        int_type y;

        if (step_search(k, x, y))
        {
            p = x;
            q = y;
            return true;
        }

        // guided search (safe direction only)
        int_type prod = x * (k / x);

        if (prod < k)
            --x;
        else
            ++x;

        if (x <= limit)
            return false;
    }

    return false;
}

} // namespace factiz
