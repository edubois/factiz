#include <factiz/factiz.hpp>
#include <iostream>
#include <chrono> // For timing
#include <boost/multiprecision/cpp_int.hpp>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }

    factiz::int_type pq = boost::multiprecision::cpp_int(argv[1]);
    factiz::int_type p, q;

    // Start the timer
    auto start = std::chrono::high_resolution_clock::now();

    bool success = factiz::factorize(pq, p, q);

    // Stop the timer and calculate duration
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Output results
    if (success)
    {
        std::cout << "P*Q = " << p << " * " << q << std::endl;
    }
    else
    {
        std::cout << "Failed to factorize (or input is prime/trivial)." << std::endl;
    }

    // Print the time taken
    std::cerr << "Time taken: " << duration.count() << " ms" << std::endl;

    return 0;
}

