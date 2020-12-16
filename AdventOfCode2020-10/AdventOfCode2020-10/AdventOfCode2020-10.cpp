// AdventOfCode2020-10.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>

int main()
{
    unsigned int max = 0;
    std::vector<unsigned int> adapters;
    for (unsigned int input; std::cin >> input;)
    {
        adapters.push_back(input);
        max = std::max(max, input);
    }
    adapters.push_back(0);      // First
    adapters.push_back(max+3);  // Last

    std::sort(adapters.begin(), adapters.end());

    // Tribonacci sequence - Adding the results from the last 3 chargers (conditionally). 
    // The number of posible ways we can get to a adapter is the sum of all the possible ways we can get to our previous adapters(s). That can be between 1 and 3 adapters given the problems conditions.
    std::vector<unsigned long long> solutions;
    solutions.push_back(1); // Only one way to get to the first one
    for (unsigned int i = 1; i < adapters.size(); ++i)
    {
        unsigned long long result = 0;
        if (i >= 1 && adapters[i] - adapters[i - 1] < 4)
        {
            result += solutions[i - 1];
        }
        if (i >= 2 && adapters[i] - adapters[i - 2] < 4)
        {
            result += solutions[i - 2];
        }
        if (i >= 3 && adapters[i] - adapters[i - 3] < 4)
        {
            result += solutions[i - 3];
        }
        solutions.push_back(result);
    }

    std::cout << solutions.back() << "\n";
}