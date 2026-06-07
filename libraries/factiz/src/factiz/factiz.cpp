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

inline int_type isqrt(const int_type& n)
{
    return sqrt(n);
}

/*
 * Fermat-style initialization:
 * start near sqrt(k)
 */
void init_point(const int_type& k, int_type& x, int_type& y)
{
    x = isqrt(k);
    y = x;
}

/*
 * Newton-like hyperbola descent.
 *
 * We solve:
 *      xy = k
 *
 * using error:
 *      E = k - xy
 *
 * and derivative:
 *      d(xy) = y dx + x dy
 */
bool follow(const int_type& k, int_type& x, int_type& y)
{
    const int_type limit = 1;

    for (int iter = 0; iter < 200000000; ++iter)
    {
        int_type prod = x * y;

        if (prod == k)
            return true;

        int_type E = k - prod;

        /*
         * If error is positive:
         * product too small → increase variables
         */
        if (E > 0)
        {
            // best axis move estimate
            int_type dy = E / x;

            if (dy == 0)
                dy = 1;

            y += dy;
        }
        else
        {
            // product too large → decrease x
            int_type dx = (-E) / y;

            if (dx == 0)
                dx = 1;

            x -= dx;
        }

        if (x <= limit || y <= limit)
            return false;
    }

    return false;
}

} // namespace

bool factorize(const int_type& k, int_type& p, int_type& q)
{
    p = 0;
    q = 0;

    int_type x, y;
    init_point(k, x, y);

    /*
     * Fermat symmetry:
     * try both directions implicitly via swap
     */
    if (follow(k, x, y))
    {
        p = x;
        q = y;
        return true;
    }

    // fallback: reversed roles
    init_point(k, x, y);

    if (follow(k, x, y))
    {
        p = x;
        q = y;
        return true;
    }

    return false;
}

} // namespace factiz
