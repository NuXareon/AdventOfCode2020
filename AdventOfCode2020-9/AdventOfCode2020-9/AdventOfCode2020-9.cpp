// AdventOfCode2020-9.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>

// Recursive function to keep adding number until we reach (success) or exceed target value or index (fail)
bool AddToTarget(unsigned int i, unsigned int target, unsigned long long sum, const std::vector<unsigned long long>& values, std::vector<unsigned long long>& result)
{
    if (i == target)
    {
        return false;
    }

    sum += values[i];
    
    if (sum > values[target])
    {
        return false;
    }

    if (sum == values[target])
    {
        result.push_back(values[i]);
        return true;
    }

    if (sum < values[target])
    {
        if (AddToTarget(i+1, target, sum, values, result))
        {
            result.push_back(values[i]);
            return true;
        }
    }


    return false;
}

int main()
{
    std::vector<unsigned long long> values;
    std::vector<unsigned long long> results;
    for (unsigned long long input; std::cin >> input;)
    {
        values.push_back(input);
    }

    for (int i = 25; i < values.size(); ++i)
    {
        // Part 1 - Find number which is not the sum of 2 of the previous 25 numbers 
        bool isValid = false;
        for (int j = std::max(i - 25, 0); j < i; ++j)
        {
            for (int k = j+1; k < i; ++k)
            {
                if (values[i] == values[j] + values[k])
                {
                    isValid = true;
                    break;
                }
            }

            if (isValid)
            {
                break;
            }
        }

        // Part 2 - Find contiguous number that add to previous number
        if (!isValid)
        {
            for (unsigned int j = 0; j < values.size(); ++j)
            {
                if (AddToTarget(j, i, 0, values, results))
                {
                    break;
                }
            }

            break;
        }
    }

    std::sort(results.begin(), results.end());

    std::cout << results.front() + results.back() << "\n";
}