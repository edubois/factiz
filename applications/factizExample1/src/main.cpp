#include <factiz/factiz.hpp>
#include <iostream>
#include <sstream>

int main(int argc, char** argv)
{
    if ( argc != 2 )
    {
        std::cerr << "Please give a product of two prime numbers (P*Q)..." << std::endl;
        return -1;
    }
    factiz::int_type pq = atoi( argv[1] );
    factiz::int_type p, q;
    factiz::factorize( pq, p, q );
    std::cout << "P*Q = " << p << " * " << q << std::endl;
    return 0;
}

