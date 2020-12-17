// AdventOfCode2020-12.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cmath>

// clockwise
std::pair<int, int> RotateRight(int x, int y)
{
    int temp = y;
    y = -x;
    x = temp;

    return { x, y };
}

// counterclockwise
std::pair<int, int> RotateLeft(int x, int y)
{
    int temp = -y;
    y = x;
    x = temp;

    return { x, y };
}


int main()
{
    int waypointX = 10;
    int waypointY = 1;
    int shipX = 0;
    int shipY = 0;

    for (char action; std::cin >> action;)
    {
        unsigned int value;
        std::cin >> value;

        switch (action)
        {
            case 'N':
            {
                waypointY += value;
                break;
            }
            case 'S':
            {
                waypointY -= value;
                break;
            }
            case 'E':
            {
                waypointX += value;
                break;
            }
            case 'W':
            {
                waypointX -= value;
                break;
            }
            case 'L':
            {
                auto shift = value / 90;
                for (unsigned int i = 0; i < shift; ++i)
                {
                    std::tie(waypointX, waypointY) = RotateLeft(waypointX, waypointY);
                }
                break;
            }
            case 'R':
            {
                auto shift = value / 90;
                for (unsigned int i = 0; i < shift; ++i)
                {
                    std::tie(waypointX, waypointY) = RotateRight(waypointX, waypointY);
                }
                break;
            }
            case 'F':
            {
                for (unsigned int i = 0; i < value; ++i)
                {
                    shipX += waypointX;
                    shipY += waypointY;
                }
                break;
            }
        }
    }

    std::cout << std::abs(shipX)+std::abs(shipY) << "\n";
}
