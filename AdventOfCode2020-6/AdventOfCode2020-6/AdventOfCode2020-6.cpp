// AdventOfCode2020-6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <set>
#include <algorithm>

int main()
{
    unsigned int peopleInGroup = 0;
    unsigned int result = 0;
    std::multiset<char> answers;

    for (std::string input; std::getline(std::cin, input);)
    {
        if (input == "")
        {
            for (auto answerIt = answers.begin(); answerIt != answers.end(); answerIt = std::upper_bound(answerIt, answers.end(), *answerIt))
            {
                if (answers.count(*answerIt) == peopleInGroup)
                {
                    ++result;
                }

            }

            answers.clear();
            peopleInGroup = 0;
        }
        else
        {
            for (const char& answer : input)
            {
                answers.insert(answer);
            }
            ++peopleInGroup;
        }
    }

    // Process last group
    for (auto answerIt = answers.begin(); answerIt != answers.end(); answerIt = std::upper_bound(answerIt, answers.end(), *answerIt))
    {
        if (answers.count(*answerIt) == peopleInGroup)
        {
            ++result;
        }
    }

    std::cout << result << "\n";
}