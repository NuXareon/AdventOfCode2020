// AdventofCode2020-2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

int main()
{
    unsigned int rangeMin, rangeMax;
    std::string password;
    char letter, separators;
    unsigned int validPasswords = 0;

    while (std::cin >> rangeMin)
    {
        std::cin >> separators;
        std::cin >> rangeMax;
        std::cin >> letter;
        std::cin >> separators;
        std::cin >> password;

        if (rangeMax-1 > password.size())
        {
            continue;
        }

        unsigned int letterCount = 0;
        if (password[rangeMin-1] == letter)
        {
            ++letterCount;
        }

        if (password[rangeMax-1] == letter)
        {
            ++letterCount;
        }

        if (letterCount == 1)
        {
            ++validPasswords;
        }
    }

    std::cout << "Valid passwords: " << validPasswords << "\n";
}