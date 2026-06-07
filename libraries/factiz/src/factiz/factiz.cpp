#include "factiz.hpp"
#include <iostream>
#include <future>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>

namespace boost { namespace multiprecision {
inline cpp_int abs(const cpp_int &x) { return x * x.sign(); }
}}

namespace factiz { namespace {
using namespace boost::multiprecision;

// OPT5: abs natif plus efficace
inline int_type fast_abs(const int_type &x) { return (x < 0) ? -x : x; }

void lc(int_type &l, const int_type &x1, const int_type &x2)
{ int_type s=x2-x1,d=s/4; l=d+x1; l*=l; }

void rc(int_type &r, const int_type &x1, const int_type &x2)
{ int_type s=x2-x1,d=s/2,sm=d+x1,t=sm*sm; t*=2;
  int_type t2=0; lc(t2,x1,x2); r=t-t2; }

void factorization_impl(const int_type &k, int_type &l, int_type &h)
{
    // OPT6: démarrer depuis sqrt(k) — fenêtre initiale bien plus petite
    l = sqrt(k);
    h = l + 1;
    while (h >= l) {
        int_type s=h-l, d=s/2, t1=0; lc(t1,l,h); t1=fast_abs(k-t1);
        int_type t2=0; rc(t2,l,h); t2=fast_abs(k-t2);
        if (t2>t1) { h=h-d; if(l>h){++h;} } else { ++l; }
    }
}

// OPT1: mise à jour incrémentale des produits (2 mult/iter au lieu de 4)
void follow_impl(const int_type &k, int_type &x, int_type &y,
                 const int dx, const int dy,
                 const int_type &w, const int_type &h)
{
    int_type xi=x+dx, yi=y+dy;
    int_type xy=x*y, xiy=xi*y, xyi=x*yi, xiyi=xi*yi;
    do {
        int_type ll=fast_abs(xy-k), hl=fast_abs(xyi-k),
                 lh=fast_abs(xiy-k), hh=fast_abs(xiyi-k);
        if (ll==0) break;
        else if (hl>hh && lh>hh) {
            // pas diagonal
            x+=dx; y+=dy; xi=x+dx; yi=y+dy;
            xy=xiyi; xiy=xi*y; xyi=x*yi; xiyi=xi*yi; // 3 mult
        }
        else if (hl>lh) {
            // pas en x
            x+=dx; xi=x+dx;
            xy=xiy; xyi=xiyi; xiy=xi*y; xiyi=xi*yi;  // 2 mult
        }
        else {
            // pas en y
            y+=dy; yi=y+dy;
            xy=xyi; xiy=xiyi; xyi=x*yi; xiyi=xi*yi;  // 2 mult
        }
    } while (w>xi && xi>1 && h>yi && yi>1);
}
} // namespace

bool factorize(const int_type &pq, int_type &p, int_type &q)
{
    int_type hl, hh; p=0; q=0;
    factorization_impl(pq, hl, hh);

    // OPT3: les deux directions en parallèle
    auto fut = std::async(std::launch::async, [pq, hl, hh]() {
        int_type x=hl, y=hh;
        follow_impl(pq, x, y, 1, -1, pq, pq);
        return std::make_pair(x, y);
    });

    int_type x2=hl, y2=hh;
    follow_impl(pq, x2, y2, -1, 1, pq, pq);
    auto [x1,y1] = fut.get();

    // OPT4: affichage uniquement en fin, hors boucle chaude
    if (x1*y1==pq) { p=x1; q=y1; return true; }
    if (x2*y2==pq) { p=x2; q=y2; return true; }
    return false;
}
} // namespace factiz

