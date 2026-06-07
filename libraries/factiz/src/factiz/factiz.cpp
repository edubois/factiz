#include "factiz.hpp"
#include <boost/multiprecision/cpp_int.hpp>

namespace factiz
{

using int_type = boost::multiprecision::cpp_int;

namespace
{

// =========================
// UTIL
// =========================
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

// Integer log2 approximation (number of bits - 1)
inline int ilog2(const int_type& x)
{
    if (x <= 1) return 0;
    int bits = 0;
    int_type tmp = x;
    while (tmp > 1)
    {
        tmp >>= 1;
        bits++;
    }
    return bits;
}

// =========================
// LEARNED HASH FUNCTION (Forward-Forward inspired)
// =========================
// Weights are placeholders for values that could be learned via Forward-Forward.
// These are hardcoded here but could be replaced with trained weights.
// Features: log2(x), log2(y), log2(|x-y|), n % 1000, (n/1000) % 1000, and differences.
static constexpr int W_LOGX = 3;
static constexpr int W_LOGY = 5;
static constexpr int W_LOGD = 7;
static constexpr int W_MOD1 = 11;
static constexpr int W_MOD2 = 13;
static constexpr int W_DIFF_XY = 17;
static constexpr int W_DIFF_XD = 19;
static constexpr int W_DIFF_YD = 23;

inline int learned_hash(const int_type& x, const int_type& y, const int_type& n)
{
    int_type d = abs_int(x - y);

    // Feature 1: Bit lengths (log2 approximations)
    int bx = ilog2(x);
    int by = ilog2(y);
    int bd = ilog2(d);

    // Feature 2: Modulo features
    int mod1 = (n % 1000).convert_to<int>();
    int mod2 = ((n / 1000) % 1000).convert_to<int>();

    // Feature 3: Differences in bit lengths
    int diff_xy = bx - by;
    int diff_xd = bx - bd;
    int diff_yd = by - bd;

    // Weighted combination (simulates a learned linear layer)
    int hash_val = W_LOGX * bx + W_LOGY * by + W_LOGD * bd +
                   W_MOD1 * mod1 + W_MOD2 * mod2 +
                   W_DIFF_XY * diff_xy + W_DIFF_XD * diff_xd + W_DIFF_YD * diff_yd;

    // Map to bucket range [0, BUCKETS-1]
    const int BUCKETS = 256;
    return (hash_val % BUCKETS + BUCKETS) % BUCKETS; // Ensure non-negative
}

// =========================
// WALKER
// =========================
inline int_type walker_mix(int_type x, const int_type& n)
{
    return (x * x + 7 * x + 11) % n;
}

// =========================
// RHO MODES
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
// BANDIT TABLE
// =========================
static const int MODES = 5;
static const int BUCKETS = 256;

struct BanditTable
{
    int_type score[BUCKETS][MODES];

    BanditTable()
    {
        for (int i = 0; i < BUCKETS; i++)
            for (int j = 0; j < MODES; j++)
                score[i][j] = 1;
    }

    int pick(int bucket)
    {
        int best = 0;
        int_type best_score = -1;
        for (int m = 0; m < MODES; m++)
        {
            if (score[bucket][m] > best_score)
            {
                best_score = score[bucket][m];
                best = m;
            }
        }
        return best;
    }

    void reward(int bucket, int mode, int_type r)
    {
        score[bucket][mode] += (r > 0 ? 1 : -1);
    }
};

// =========================
// STREAM
// =========================
bool rho_stream(
    const int_type& n,
    int_type x0,
    int_type c,
    int_type& factor,
    BanditTable& bandit)
{
    int_type x = x0;
    int_type y = x0;

    for (int i = 0; i < 50000; i++)
    {
        int bucket = learned_hash(x, y, n);
        int mode = bandit.pick(bucket % BUCKETS);

        auto f = [&](int_type v)
        {
            return rho_f(v, c, mode, n);
        };

        x = f(x);
        y = f(f(y));

        int_type d = gcd_int(abs_int(x - y), n);

        if (d > 1 && d < n)
        {
            bandit.reward(bucket % BUCKETS, mode, 50);
            factor = d;
            return true;
        }

        if (d == n)
        {
            bandit.reward(bucket % BUCKETS, mode, -5);
            return false;
        }

        // Forward-Forward style shaping
        if (i % 32 == 0)
        {
            bandit.reward(bucket % BUCKETS, mode, 1);
        }
    }
    return false;
}

// =========================
// DRIVER
// =========================
bool rho_lsh_ff(const int_type& n, int_type& factor)
{
    BanditTable bandit;
    int_type base = n % 100000;

    for (int attempt = 0; attempt < 200; attempt++)
    {
        int_type seed = walker_mix(base + attempt * 1337, n);
        int_type x0 = (seed % (n - 2)) + 2;
        int_type c  = walker_mix(seed + 17, n);
        if (c == 0) c = 1;

        if (rho_stream(n, x0, c, factor, bandit))
            return true;
    }
    return false;
}

} // namespace

// =========================
// PUBLIC INTERFACE
// =========================
bool factorize(const int_type& n, int_type& p, int_type& q)
{
    int_type f;
    if (rho_lsh_ff(n, f))
    {
        p = f;
        q = n / f;
        return true;
    }
    return false;
}

} // namespace factiz

