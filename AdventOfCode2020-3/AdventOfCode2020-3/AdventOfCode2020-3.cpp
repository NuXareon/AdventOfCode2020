// AdventOfCode2020-3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> map;
    for (std::string row; std::cin >> row;)
    {
        map.emplace_back(std::move(row));
    }

    auto treeCount = [mapWidth = map[0].size(), mapHeight = map.size(), &map](unsigned long long xStart, unsigned long long yStart)
    {
        unsigned long long trees = 0;
        for (unsigned long long x = xStart, y = yStart; y < mapHeight; y += yStart, x = (x + xStart) % mapWidth)
        {
            if (map[y][x] == '#')
            {
                ++trees;
            }
        }
        return trees;
    };

    unsigned long long trees1 = treeCount(1, 1);
    unsigned long long trees2 = treeCount(3, 1);
    unsigned long long trees3 = treeCount(5, 1);
    unsigned long long trees4 = treeCount(7, 1);
    unsigned long long trees5 = treeCount(1, 2);

    std::cout << trees1*trees2*trees3*trees4*trees5 << "\n";
}