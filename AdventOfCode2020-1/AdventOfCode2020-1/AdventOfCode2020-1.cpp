// AdventOfCode2020-1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>

int main()
{
    // Input
    std::vector<unsigned long long> entries;

    for (unsigned long long inputEntry; std::cin >> inputEntry;)
    {
        entries.emplace_back(inputEntry);
    }

    // Solving
    unsigned long long result = 0;
    std::sort(entries.begin(), entries.end());

    for (unsigned long long x : entries)
    {
        for (auto it = entries.begin(); it != entries.end(); ++it)
        {
            // Binary search for the last value
            // Find a value that adds to the target
            size_t highLimit = entries.size();
            size_t lowLimit = std::distance(entries.begin(), it);

            while (highLimit != lowLimit)
            {
                const size_t positionToCheck = (lowLimit + highLimit) / 2;
                const unsigned long long sum = x  + *it + entries[positionToCheck];
                if (sum == 2020)
                {
                    result = x * *it * entries[positionToCheck];
                    break;
                }
                else if (sum < 2020)
                {
                    if (lowLimit == positionToCheck)
                    {
                        break;
                    }
                    lowLimit = positionToCheck;
                }
                else
                {
                    if (highLimit == positionToCheck)
                    {
                        break;
                    }
                    highLimit = positionToCheck;
                }
            }

            if (result != 0)
            {
                break;
            }
        }
    }    

    std::cout << "Result: " << result << "\n";
}