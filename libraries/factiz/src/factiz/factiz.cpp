#include "factiz.hpp"
#include <iostream>

namespace factiz
{

namespace
{

// Left classifier
void lc( int_type & l, const int_type & x1, const int_type & x2 )
{
    /* Apply l = (((x2-x1)*0.25) + x1 )^2 */

    /* Substract low bound from high bound */
    int_type sub1 = x2 - x1;
    /* Divide by 4 */
    int_type div1 = sub1 / 4;
    /* Add low bound */
    l = div1 + x1;
    /* Powerize to 2 */
    l *= l;
}

// Right classifier
void rc( int_type & r, const int_type & x1, const int_type & x2 )
{
    /* Apply: ((0.75 * (x2-x1)) + x1)^2 +
              ((2*(0.5  * (x2-x1)) + x1)^2) - lc(x1, x2) -
              ((0.75 * (x2-x1)) + x1)^2 */
    /* Substract low bound from high bound */
    int_type sub1 = x2 - x1;
    /* Divide by 2 */
    int_type div1 = sub1 / 2;
    /* Add low bound */
    int_type sum1 = div1 + x1;
    /* Powerize to 2 */
    int_type tmp1 = sum1 * sum1;
    /* Multiply by 2 */
    tmp1 *= 2;
    /* Left classifier */
    int_type tmp2 = 0;
    lc( tmp2, x1, x2 );
    /* Add low bound */
    r = tmp1 - tmp2;
}

void factorization_impl( const int_type & k, int_type & l, int_type & h )
{
    l = 0;
    h = k;
    // Continue until granularity is not unity
    while ( h >= l )
    {
        /* h - l */
        int_type sub1 = h - l;
        /* Divide by 2 */
        int_type div1 = sub1 / 2;
        /* euclidian left part */
        int_type tmp1 = 0;
        lc( tmp1, l, h );
        tmp1 = abs( k - tmp1 );
        int_type tmp2 = 0;
        rc( tmp2, l, h );
        tmp2 = abs( k - tmp2 );
        if ( tmp2 > tmp1 )
        {
            h = h - div1;
            if ( l > h )
            {
                ++h;
            }
        }
        else
        {
            ++l;
        }
    }
}

void follow_impl(const int_type & k,
                 int_type & x, int_type & y,
                 const int dx, const int dy,
                 const int_type & w, const int_type & h)
{
    int_type c, lh, hh, hl, ll, xi, yi, xy, xiy, xyi, xiyi, iix, iiy;
    /* Initial warmup */
    lc( c, x, y );
    iix = dx;
    iiy = dy;
    xi  = x  + iix;
    yi  = y  + iiy;
    xy  = x  * y;
    xiy = xi * y;
    xyi =  x * yi;
    xiyi = xi * yi;

    do
    {
        /* compute ll */
        ll = abs( xy - k );
        /* compute hl */
        hl = abs( xyi - c );
        /* compute lh */
        lh = abs( xiy - c );
        /* compute hh */
        hh = abs( xiyi - c );

        /* Check if found */
        if ( ll == 0 )
        {
            break;
        }
        else if ( hl > hh && lh > hh )
        {
            x += iix;
            y += iiy;
        }
        else if ( hl > lh )
        {
            x += iix;
        }
        else
        {
            y += iiy;
        }
        xi = x + iix;
        yi = y + iiy;
        xy = x * y;
        xiy = xi * y;
        xyi = x * yi;
        xiyi = xi * yi;
    } while( w > xi && xi > 1 && h > yi && yi > 1 );
}

}

/**
 * @brief factorize the product of two bigs prime numbers
 * @param pq p*q input number
 * @param p output p
 * @param q output q
 * @return true if success
 */
bool factorize( const int_type & pq, int_type & p, int_type & q )
{
    int_type hl, hh; // Hyperbol bounds (low bound and high bound)
    
    p = 0; q = 0;
    factorization_impl( pq, hl, hh);
    
    std::cerr << "Analysis hyperbol direction: up-right" << std::endl;
    int_type x, y;
    x = hl; y = hh;
    follow_impl( pq, x, y, 1, -1, pq, pq );
    int_type pq_candidate = x * y;
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    // Success ?
    if ( pq_candidate != pq )
    {
        std::cerr << "Analysis hyperbol direction: down-left" << std::endl;
        x = hl; y = hh;
        follow_impl( pq, x, y, -1, 1, pq, pq );
        pq_candidate = x * y;
        std::cout << "x: " << x << std::endl;
        std::cout << "y: " << y << std::endl;
    }

    // Success (should be) ?
    if ( pq_candidate == pq )
    {
        p = x;
        q = y;
        return true;
    }
    return false;
}

}
