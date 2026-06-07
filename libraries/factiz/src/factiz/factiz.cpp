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

inline int_type mod(int_type a, const int_type& n)
{
    return a % n;
}

inline int_type mulmod(int_type a, int_type b, const int_type& n)
{
    return (a * b) % n;
}

/* =========================
   POLLARD RHO (Brent-style simplified)
   ========================= */
bool rho_stage(const int_type& n, int_type& factor)
{
    auto f = [&](int_type x, int_type c)
    {
        return (x * x + c) % n;
    };

    std::mt19937_64 rng(1234);

    for (int attempt = 0; attempt < 20; ++attempt)
    {
        int_type x = rng() % (n - 2) + 2;
        int_type y = x;
        int_type c = rng() % (n - 1);
        if (c == 0) c = 1;

        int_type d = 1;

        while (d == 1)
        {
            x = f(x, c);
            y = f(f(y, c), c);

            d = gcd_int(abs_int(x - y), n);

            if (d == n)
                break;
        }

        if (d > 1 && d < n)
        {
            factor = d;
            return true;
        }
    }

    return false;
}

/* =========================
   ECM (very simplified Montgomery-like toy ECM core)
   ========================= */

struct Point
{
    int_type x;
    int_type y;
};

/*
 * This is NOT full ECM implementation (that is huge),
 * but a minimal "ECM-like failure detector core".
 */
bool ecm_stage(const int_type& n, int_type& factor)
{
    std::mt19937_64 rng(5678);

    for (int attempt = 0; attempt < 30; ++attempt)
    {
        int_type x = rng() % n;
        int_type y = rng() % n;

        int_type a = rng() % n;

        // curve: y^2 = x^3 + ax + 1 mod n
        auto add = [&](Point P, Point Q) -> Point
        {
            Point R;

            int_type dx = (Q.x - P.x);
            int_type dy = (Q.y - P.y);

            int_type inv = gcd_int(dx, n);

            if (inv != 1 && inv != n)
            {
                factor = inv;
                return {0, 0};
            }

            R.x = (dy * dy - P.x - Q.x) % n;
            R.y = (dy * (P.x - R.x) - P.y) % n;

            return R;
        };

        Point P = {x, y};

        for (int i = 0; i < 200; ++i)
        {
            P = add(P, P);
        }
    }

    return false;
}

/* =========================
   HYBRID GATEWAY
   ========================= */

bool hybrid_factor(const int_type& n, int_type& p, int_type& q)
{
    int_type f;

    // Stage 1: fast wins
    if (rho_stage(n, f))
    {
        p = f;
        q = n / f;
        return true;
    }

    // Stage 2: ECM fallback
    if (ecm_stage(n, f))
    {
        p = f;
        q = n / f;
        return true;
    }

    return false;
}

} // namespace

bool factorize(const int_type& n, int_type& p, int_type& q)
{
    return hybrid_factor(n, p, q);
}

} // namespace factiz

