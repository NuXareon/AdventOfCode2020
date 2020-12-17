// CodeOfAdvent2020-5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>

int main()
{
    std::vector<unsigned int> ids;
    for (std::string input; std::cin >> input;)
    {
        unsigned int bottomRow = 0;
        unsigned int topRow = 127;
        unsigned int bottomColumn = 0;
        unsigned int topColumn = 7;
        for (const char& letter : input)
        {
            if (letter == 'F')
            {
                topRow = (topRow + bottomRow) / 2;
            }
            else if (letter == 'B')
            {
                bottomRow = static_cast<unsigned int>(std::ceilf(static_cast<float>(topRow + bottomRow) / 2.0f));
            }
            else if (letter == 'L')
            {
                topColumn = (topColumn + bottomColumn) / 2;
            }
            else if (letter == 'R')
            {
                bottomColumn = static_cast<unsigned int>(std::ceilf(static_cast<float>(topColumn + bottomColumn) / 2.0f));
            }
        }

        assert(topRow == bottomRow && topColumn == bottomColumn);
        ids.push_back(topRow * 8 + topColumn);
    }

    std::sort(ids.begin(), ids.end());
    
    unsigned int result = -1;

    for (unsigned int i = ids.front(); i < 1024; ++i)
    {
        if (std::find(ids.begin(), ids.end(), i) == ids.end())
        {
            if (std::find(ids.begin(), ids.end(), i - 1) != ids.end()
                && (std::find(ids.begin(), ids.end(), i + 1) != ids.end()))
            {
                result = i;
                break;
            }
        }
    }

    std::cout << result << "\n";
}