// AdventOfCode2020-7.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <set>

struct Bag
{
    struct ChildLink
    {
        unsigned int capacity;
        size_t bagIndex;
    };

    unsigned int index;
    std::string color;
    std::vector<ChildLink> child;
};

void LinkBags(Bag& parent, Bag& child, unsigned int capacity)
{
    auto& newChildLink = parent.child.emplace_back();
    newChildLink.capacity = capacity;
    newChildLink.bagIndex = child.index;
}

// Recursive function to count the number of child bags of a bag
unsigned int GetChildBags(const Bag& bag, const std::vector<Bag>& bagColorCapacities)
{
    unsigned int result = 0;
    for (const auto& childBag : bag.child)
    {
        result += childBag.capacity * (1+GetChildBags(bagColorCapacities[childBag.bagIndex], bagColorCapacities));
    }
    return result;
}

int main()
{
    std::vector<Bag> bagColorCapacities;

    // Process input
    for (std::string input; std::getline(std::cin, input);)
    {
        std::stringstream inputStream;
        inputStream.str(input);

        std::string color1, color2;
        inputStream >> color1 >> color2;

        std::string dummy;
        inputStream >> dummy >> dummy;  // bag(s) contains 

        std::string newColor = color1 + color2;
        auto it = std::find_if(bagColorCapacities.begin(), bagColorCapacities.end(), [&newColor](const Bag& bag)
            {
                return bag.color == newColor;
            });

        size_t newBagIdx;
        if (it != bagColorCapacities.end())
        {
            newBagIdx = std::distance(bagColorCapacities.begin(), it);
        }
        else
        {
            newBagIdx = bagColorCapacities.size();
            bagColorCapacities.emplace_back();
        }

        bagColorCapacities[newBagIdx].color = newColor;
        bagColorCapacities[newBagIdx].index = newBagIdx;

        for (std::string number; inputStream >> number;)
        {
            if (!std::isdigit(number[0]))
            {
                break;
            }

            inputStream >> color1 >> color2;
            inputStream >> dummy; // bag(s)[,/.]

            newColor = color1 + color2;
            it = std::find_if(bagColorCapacities.begin(), bagColorCapacities.end(), [&newColor](const Bag& bag)
                {
                    return bag.color == newColor;
                });

            if (it != bagColorCapacities.end())
            {
                LinkBags(bagColorCapacities[newBagIdx], *it, std::stoi(number));
            }
            else 
            {
                const size_t newChildBagIdx = bagColorCapacities.size();
                auto& newChildBag = bagColorCapacities.emplace_back();
                newChildBag.color = newColor;
                newChildBag.index = newChildBagIdx;

                LinkBags(bagColorCapacities[newBagIdx], newChildBag, std::stoi(number));
            }
        }
    }

    // Find started bag and begin recursion
    auto it = std::find_if(bagColorCapacities.begin(), bagColorCapacities.end(), [](const Bag& bag)
        {
            return bag.color == "shinygold";
        }); 
    
    const unsigned int result = GetChildBags(*it, bagColorCapacities);
    
    std::cout << result << "\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
