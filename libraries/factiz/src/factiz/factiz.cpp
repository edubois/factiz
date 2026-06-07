#include "factiz.hpp"
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <vector>
#include <algorithm>

namespace boost
{
namespace multiprecision
{
inline cpp_int abs(const cpp_int &x) { return x * x.sign(); }
inline const cpp_int& max(const cpp_int &a, const cpp_int &b) { return (a > b) ? a : b; }
inline const cpp_int& min(const cpp_int &a, const cpp_int &b) { return (a < b) ? a : b; }
} // namespace multiprecision
} // namespace boost

namespace factiz
{
namespace
{
using namespace boost::multiprecision;

// Integer square root using Newton's method
int_type isqrt(const int_type &n)
{
    if (n == 0) return 0;
    int_type x = n;
    int_type y = (x + 1) / 2;
    while (y < x)
    {
        x = y;
        y = (x + n / x) / 2;
    }
    return x;
}

// Miller-Rabin primality test
bool is_prime(const int_type &n)
{
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    int_type d = n - 1;
    int s = 0;
    while (d % 2 == 0) { d /= 2; s++; }

    const std::vector<int_type> bases = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (const int_type &base : bases)
    {
        if (base >= n) continue;
        int_type a = base;
        int_type x = 1;
        int_type power = d;
        while (power > 0)
        {
            if (power % 2 == 1) x = (x * a) % n;
            a = (a * a) % n;
            power /= 2;
        }
        if (x == 1 || x == n - 1) continue;

        bool composite = true;
        for (int r = 1; r < s; r++)
        {
            x = (x * x) % n;
            if (x == n - 1) { composite = false; break; }
        }
        if (composite) return false;
    }
    return true;
}

// Find initial bounds with precise integer square root
void find_initial_bounds(const int_type &k, int_type &l, int_type &h)
{
    int_type sqrt_k = isqrt(k);
    // Dynamic buffer: 1% of sqrt(k) or at least 1000
    int_type buffer = boost::multiprecision::max(int_type(1000), sqrt_k / 100);
    l = boost::multiprecision::max(int_type(2), sqrt_k - buffer);
    h = boost::multiprecision::min(k - 1, sqrt_k + buffer);
}

// Hyperbola traversal with small steps
bool follow_hyperbola(const int_type &k, int_type &x, int_type &y)
{
    const int_type max_iterations = 1000000;
    int_type iteration = 0;

    while (iteration++ < max_iterations)
    {
        int_type product = x * y;
        if (product == k)
        {
            if (x > 1 && y > 1 && x < k && y < k) return true;
            // Skip trivial solutions
            if (x == 1) x++;
            else if (y == 1) y++;
            else if (x == k) x--;
            else if (y == k) y--;
            continue;
        }

        // Use small, fixed steps
        int_type step = (k > 1000000000) ? 10 : 1;

        if (product < k)
        {
            if (x < y) x += step;
            else y += step;
        }
        else
        {
            if (x > y && x > 2) x -= step;
            else if (y > 2) y -= step;
            else break;
        }
    }
    return false;
}

} // namespace

bool factorize(const int_type &pq, int_type &p, int_type &q)
{
    p = 0;
    q = 0;

    if (pq <= 3) return false;
    if (is_prime(pq)) return false;

    // 1. Try trial division for small factors
    for (int_type x = 2; x <= 1000000; x++)
    {
        if (pq % x == 0)
        {
            int_type candidate = pq / x;
            if (candidate > 1 && candidate < pq)
            {
                p = x;
                q = candidate;
                return true;
            }
        }
    }

    // 2. Try near sqrt(pq)
    int_type l, h;
    find_initial_bounds(pq, l, h);
    for (int_type x = l; x <= h; x++)
    {
        if (x <= 1 || x >= pq) continue;
        if (pq % x == 0)
        {
            p = x;
            q = pq / x;
            return true;
        }
    }

    // 3. Try hyperbola traversal from (2, pq/2)
    int_type x = 2;
    int_type y = pq / 2;
    if (follow_hyperbola(pq, x, y))
    {
        p = x;
        q = y;
        return true;
    }

    // 4. Try from (sqrt(pq), sqrt(pq))
    x = isqrt(pq);
    y = x;
    if (follow_hyperbola(pq, x, y))
    {
        p = x;
        q = y;
        return true;
    }

    return false;
}

} // namespace factiz
