// AdventOfCode2020-13.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>

int main()
{
    std::vector<std::pair<long long, long long>> busIds;

    char dummy;
    unsigned int extraMinutes = 0;
    unsigned long long N = 1;
    do
    {
        // If it's a valid number -> process it. Otherwise discard the 'x'.
        unsigned int busId;
        if (std::cin >> busId)
        {
            N *= busId;
            busIds.emplace_back(busId, extraMinutes);
        }
        else
        {
            std::cin.clear();
            std::cin >> dummy;
        }

        ++extraMinutes;
    } while (std::cin >> dummy);

    auto GetXi = [](unsigned long long Ni, unsigned long long ni)
    {
        for (unsigned long long i = 1;; ++i)
        {
            auto result = (Ni * i) % ni;
            if (result == 1)
            {
                return i;
            }
        }

        return 0ull;
    };

    // Chinese reminder theorem
    unsigned long long resultTimestamp = 0;
    for (const auto& busId : busIds)
    {
        // We need to invert the order of the reminder to get the actual order. This is because we want to get a number PAST the timestamp, not before.
        long long bi = (busId.first - busId.second) % busId.first; 
        if (bi < 0)
        {
            // Also make sure the modulus is positive, since C++ can return negative values if on of the operands is negative.
            bi += busId.first;
        }
        unsigned long long ni = busId.first;
        unsigned long long Ni = N / ni;
        unsigned long long xi = GetXi(Ni, ni);
        resultTimestamp += (bi * (Ni * xi) % N) % N;
        resultTimestamp %= N;
    }

    std::cout << resultTimestamp <<"\n";
}