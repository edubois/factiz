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

// =========================
// BANDIT POLICY (VERY SIMPLE)
// =========================

struct Policy
{
    int_type score = 1;
};

static const int MODES = 5;

// reward table (adaptive weights)
struct Bandit
{
    Policy p[MODES];

    int pick(int_type seed)
    {
        // weighted deterministic selection
        int best = 0;
        int_type best_score = -1;

        for (int i = 0; i < MODES; i++)
        {
            int_type s = p[i].score * (seed % (i + 1 + 1));
            if (s > best_score)
            {
                best_score = s;
                best = i;
            }
        }

        return best;
    }

    void reward(int mode, int_type r)
    {
        p[mode].score += r + 1;
    }
};

// =========================
// WALKER MIX
// =========================
inline int_type walker_mix(int_type x, const int_type& n)
{
    return (x * x + 7 * x + 11) % n;
}

// =========================
// RHO MODE FUNCTION
// =========================
inline int_type rho_f(int_type x, int_type c, int mode, const int_type& n)
{
    switch (mode)
    {
        case 0: return (x * x + c) % n;
        case 1: return (x * x + x + c) % n;
        case 2: return (x * x + 3 * x + c) % n;
        case 3: return (x * x + walker_mix(x, n) + c) % n;
        case 4: return (x * x + (x ^ c) + c) % n;
    }
    return (x * x + c) % n;
}

// =========================
// SINGLE STREAM WITH POLICY
// =========================
bool rho_stream(
    const int_type& n,
    int_type x0,
    int_type c,
    int mode,
    int_type& factor,
    Bandit& bandit)
{
    auto f = [&](int_type x)
    {
        return rho_f(x, c, mode, n);
    };

    int_type x = x0;
    int_type y = x0;

    for (int i = 0; i < 50000; i++)
    {
        x = f(x);
        y = f(f(y));

        int_type d = gcd_int(abs_int(x - y), n);

        if (d > 1 && d < n)
        {
            factor = d;

            // REWARD: strong signal
            bandit.reward(mode, 1000 / (i + 1));

            return true;
        }

        if (d == n)
        {
            // penalty (bad cycle collapse)
            bandit.reward(mode, -10);
            return false;
        }
    }

    // weak penalty
    bandit.reward(mode, -1);
    return false;
}

// =========================
// MULTI-STREAM LEARNED RHO
// =========================
bool rho_learned(const int_type& n, int_type& factor)
{
    Bandit bandit;

    int_type base = n % 100000;

    for (int attempt = 0; attempt < 30; ++attempt)
    {
        int mode = bandit.pick(base + attempt);

        int_type seed = walker_mix(base + attempt * 1337, n);

        int_type x0 = (seed % (n - 2)) + 2;
        int_type c  = walker_mix(seed + 17, n);
        if (c == 0) c = 1;

        if (rho_stream(n, x0, c, mode, factor, bandit))
            return true;
    }

    return false;
}

} // namespace

bool factorize(const int_type& n, int_type& p, int_type& q)
{
    int_type f;

    if (rho_learned(n, f))
    {
        p = f;
        q = n / f;
        return true;
    }

    return false;
}

} // namespace factiz
