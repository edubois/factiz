#include "factiz.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <random>
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

// ============================
// MULTI-STREAM RHO CORE
// ============================

struct RhoStream
{
    int_type x;
    int_type y;
    int_type c;
};

// optional: your walker influences seeds
inline int_type walker_mix(int_type seed, const int_type& n)
{
    return (seed * seed + 7 * seed + 3) % n;
}

bool rho_stream_run(const int_type& n, RhoStream& s, int_type& factor)
{
    auto f = [&](const int_type& x)
    {
        return (x * x + s.c) % n;
    };

    int_type x = s.x;
    int_type y = s.y;

    for (int iter = 0; iter < 5000000; ++iter)
    {
        x = f(x);
        y = f(f(y));

        int_type d = gcd_int(abs_int(x - y), n);

        if (d > 1 && d < n)
        {
            factor = d;
            s.x = x;
            s.y = y;
            return true;
        }

        if (d == n)
            return false;
    }

    s.x = x;
    s.y = y;
    return false;
}

// ============================
// MULTI-STREAM DRIVER
// ============================

bool rho_multi_stream(const int_type& n, int_type& factor)
{
    if (n % 2 == 0)
    {
        factor = 2;
        return true;
    }

    const int STREAMS = 16;

    // deterministic base seeds (no RNG instability)
    int_type base = n % 100000;

    for (int i = 0; i < STREAMS; ++i)
    {
        RhoStream s;

        int_type seed = walker_mix(base + i * 1337, n);

        s.x = (seed % (n - 2)) + 2;
        s.y = walker_mix(seed + 1, n);

        s.c = walker_mix(seed + 3, n);
        if (s.c == 0) s.c = 1;

        if (rho_stream_run(n, s, factor))
            return true;
    }

    return false;
}

} // namespace

bool factorize(const int_type& n, int_type& p, int_type& q)
{
    int_type f;

    if (rho_multi_stream(n, f))
    {
        p = f;
        q = n / f;
        return true;
    }

    return false;
}

} // namespace factiz
