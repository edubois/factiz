#include <factiz/factiz.hpp>
#include <iostream>
#include <chrono>
#include <boost/multiprecision/cpp_int.hpp>
#include <string>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <number> [--algo=v1|v2]" << std::endl;
        return 1;
    }

    // Default algorithm
    std::string algo = "v2";

    // Parse args
    std::string number_arg;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg.rfind("--algo=", 0) == 0)
        {
            algo = arg.substr(7);
        }
        else if (number_arg.empty())
        {
            number_arg = arg;
        }
        else
        {
            std::cerr << "Unexpected argument: " << arg << std::endl;
            return 1;
        }
    }

    factiz::int_type pq = boost::multiprecision::cpp_int(number_arg);
    factiz::int_type p, q;

    auto start = std::chrono::high_resolution_clock::now();

    bool success = false;

    if (algo == "v1")
    {
        success = factiz::factorize_v1(pq, p, q);
    }
    else if (algo == "v2")
    {
        success = factiz::factorize_v2(pq, p, q);
    }
    else
    {
        std::cerr << "Unknown algorithm: " << algo << " (use v1 or v2)" << std::endl;
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (success)
    {
        std::cout << "P * Q = " << p << " * " << q << std::endl;
    }
    else
    {
        std::cout << "Failed to factorize (or input is prime/trivial)." << std::endl;
    }

    std::cerr << "Time taken: " << duration.count() << " ms" << std::endl;

    return 0;
}

