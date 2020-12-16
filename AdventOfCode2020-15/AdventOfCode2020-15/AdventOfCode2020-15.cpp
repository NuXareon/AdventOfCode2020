// AdventOfCode2020-15.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <unordered_map>

class LastLocations
{
public:
    LastLocations() = default;
    LastLocations(int value)
    {
        InsertLast(value);
    }

    void InsertLast(int value)
    {
        previousToLast = last;
        last = value;
    }

    int GetDistance() const
    {
        return last - previousToLast;
    }

    bool HasMultipleEntries() const
    {
        return last != -1 && previousToLast != -1;
    }

private:
    int last = -1;
    int previousToLast = -1;
};

int main()
{
    // Input
    std::unordered_map<int, LastLocations> answerLastLocation;
    int previousAnswer = 0;
    for (int input; std::cin >> input;)
    {
        previousAnswer = input;
        answerLastLocation.emplace(input, answerLastLocation.size());
        // Note: this will fail for the last number. That's okay since we don't use std::cin anymore after this point.
        char comma;
        std::cin >> comma;
    }
    
    // Process
    for (unsigned int turn = answerLastLocation.size(); turn < 30000000; ++turn)
    {
        const auto it = answerLastLocation.find(previousAnswer);
        if (it != answerLastLocation.end() && it->second.HasMultipleEntries())
        {
            const int newValue = it->second.GetDistance();
            previousAnswer = newValue;
            answerLastLocation[newValue].InsertLast(turn);
        }
        else
        {
            previousAnswer = 0;
            answerLastLocation[0].InsertLast(turn);
        }
    }

    std::cout << previousAnswer << "\n";
}