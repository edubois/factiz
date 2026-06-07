#include "factiz.hpp"
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>

namespace boost
{
namespace multiprecision
{
inline cpp_int abs(const cpp_int &x) { return x * x.sign(); }
inline const cpp_int& max(const cpp_int &a, const cpp_int &b) { return (a > b) ? a : b; }
inline const cpp_int& min(const cpp_int &a, const cpp_int &b) { return (a < b) ? a : b; }
}
}

namespace factiz
{

namespace
{
using namespace boost::multiprecision;

// Left classifier (original intent: 25% weighted midpoint, squared)
void lc(int_type &l, const int_type &x1, const int_type &x2)
{
    int_type sub1 = x2 - x1;
    int_type div1 = sub1 / 4;
    l = div1 + x1;
    l *= l; // Squaring is part of your original geometric idea
}

// Right classifier (original intent: 75% weighted midpoint, squared, minus lc)
void rc(int_type &r, const int_type &x1, const int_type &x2)
{
    int_type sub1 = x2 - x1;
    int_type div1 = sub1 / 2;
    int_type sum1 = div1 + x1;
    int_type tmp1 = sum1 * sum1;
    tmp1 *= 2;
    int_type tmp2 = 0;
    lc(tmp2, x1, x2);
    r = tmp1 - tmp2;
}

void factorization_impl(const int_type &k, int_type &l, int_type &h)
{
    l = 0;
    h = k;
    while (h >= l)
    {
        int_type sub1 = h - l;
        int_type div1 = sub1 / 2;
        int_type tmp1 = 0;
        lc(tmp1, l, h);
        tmp1 = abs(k - tmp1);
        int_type tmp2 = 0;
        rc(tmp2, l, h);
        tmp2 = abs(k - tmp2);
        if (tmp2 > tmp1)
        {
            h = h - div1;
            if (l > h) { ++h; }
        }
        else
        {
            ++l;
        }
    }
}

void follow_impl(const int_type &k,
                 int_type &x, int_type &y,
                 const int dx, const int dy,
                 const int_type &w, const int_type &h)
{
    int_type xi = x + dx;
    int_type yi = y + dy;
    int_type xy = x * y;
    int_type xiy = xi * y;
    int_type xyi = x * yi;
    int_type xiyi = xi * yi;

    do
    {
        int_type ll = abs(xy - k);
        int_type hl = abs(xyi - k);  // FIXED: Compare to k (not c)
        int_type lh = abs(xiy - k);  // FIXED: Compare to k (not c)
        int_type hh = abs(xiyi - k); // FIXED: Compare to k (not c)

        if (ll == 0)
        {
            break;
        }
        else if (hl > hh && lh > hh)
        {
            x += dx;
            y += dy;
        }
        else if (hl > lh)
        {
            x += dx;
        }
        else
        {
            y += dy;
        }
        xi = x + dx;
        yi = y + dy;
        xy = x * y;
        xiy = xi * y;
        xyi = x * yi;
        xiyi = xi * yi;
    } while (w > xi && xi > 1 && h > yi && yi > 1);
}

} // namespace

bool factorize(const int_type &pq, int_type &p, int_type &q)
{
    int_type hl, hh;
    p = 0;
    q = 0;
    factorization_impl(pq, hl, hh);

    // Try direction 1: up-right (x++, y--)
    std::cerr << "Analysis hyperbol direction: up-right" << std::endl;
    int_type x = hl;
    int_type y = hh;
    follow_impl(pq, x, y, 1, -1, pq, pq);
    int_type pq_candidate = x * y;
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;

    // Try direction 2: down-left (x--, y++)
    if (pq_candidate != pq)
    {
        std::cerr << "Analysis hyperbol direction: down-left" << std::endl;
        x = hl;
        y = hh;
        follow_impl(pq, x, y, -1, 1, pq, pq);
        pq_candidate = x * y;
        std::cout << "x: " << x << std::endl;
        std::cout << "y: " << y << std::endl;
    }

    if (pq_candidate == pq)
    {
        p = x;
        q = y;
        return true;
    }
    return false;
}

} // namespace factiz
