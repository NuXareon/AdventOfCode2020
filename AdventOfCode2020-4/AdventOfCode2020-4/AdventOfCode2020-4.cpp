// AdventOfCode2020-4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <string_view>
#include <cctype>
#include <sstream>
#include <optional>

// With templates we can't assing the value to the correct variable, so we just return it and deal with it later.
// We could create a custom class for each object with their own validation, but that makes the whole thing even more complex.
template <class T>
std::optional<T> ParseValue(std::string_view inpuView, std::string_view word)
{
    const std::string_view keyword(word); 
    const std::string::size_type n = inpuView.find(keyword);
    if (n != std::string::npos)
    {
        const std::string_view view(inpuView);
        std::stringstream stream;
        stream << view.substr(n + keyword.size());

        T value;
        char separator;
        stream >> separator;
        stream >> value;

        return value;
    }

    return std::nullopt;
}

#define CHECKANDSET(word) word |= checkWord(#word)

// The nice thing about this macros is that you just need to input the name of the field you want to check.
// They will then require a valid validation function for that field (word##IsValid(value)) and will update the variable with the same name.
#define CHECKANDVALIDATEINT(word) \
    { \
        const std::string_view keyword(#word); \
        const std::string::size_type n = inpuView.find(keyword); \
        if (n != std::string::npos) \
        { \
            std::stringstream stream; \
            const std::string_view view(inpuView); \
            stream << view.substr(n + keyword.size()); \
            unsigned int value; \
            char separator; \
            stream >> separator; \
            stream >> value; \
            if (word##IsValid(value)) \
            { \
                word = true; \
            } \
        } \
    } \

#define CHECKANDVALIDATESTRING(word) \
    { \
        const std::string_view keyword(#word); \
        const std::string::size_type n = inpuView.find(keyword); \
        if (n != std::string::npos) \
        { \
            std::stringstream stream; \
            const std::string_view view(inpuView); \
            stream << view.substr(n + keyword.size()); \
            std::string value; \
            char separator; \
            stream >> separator; \
            stream >> value; \
            if (word##IsValid(value)) \
            { \
                word = true; \
            } \
        } \
    }


#define CHECKANDVALIDATEINTSTRING(word) \
    { \
        const std::string_view keyword(#word); \
        const std::string::size_type n = inpuView.find(keyword); \
        if (n != std::string::npos) \
        { \
            std::stringstream stream; \
            const std::string_view view(inpuView); \
            stream << view.substr(n + keyword.size()); \
            unsigned int value; \
            std::string units; \
            char separator; \
            stream >> separator; \
            stream >> value; \
            stream >> units; \
            if (word##IsValid(value, units)) \
            { \
                word = true; \
            } \
        } \
    }

struct DocumentChecklist
{
    bool byr = false;
    bool iyr = false;
    bool eyr = false;
    bool hgt = false;
    bool hcl = false;
    bool ecl = false;
    bool pid = false;
    bool cid = false;   // optional

    bool byrIsValid(unsigned int value) const
    {
        if (value < 1920 || value > 2002)
        {
            return false;
        }
        return true;
    }

    bool iyrIsValid(unsigned int value) const
    {
        if (value < 2010 || value > 2020)
        {
            return false;
        }
        return true;
    }

    bool eyrIsValid(unsigned int value) const
    {
        if (value < 2020 || value > 2030)
        {
            return false;
        }
        return true;
    }

    bool hgtIsValid(unsigned int value, std::string_view unit) const
    {
        if (unit == "cm")
        {
            return value >= 150 && value <= 193;
        }
        else if (unit == "in")
        {
            return value >= 59 && value <= 76;
        }

        return false;
    }

    bool hclIsValid(std::string_view value) const
    {
        // 6 digits + #
        if (value.size() != 7)
        {
            return false;
        }

        for (bool first = true; const char& letter : value)
        {
            if (first)
            {
                if (letter != '#')
                {
                    return false;
                }
                first = false;
            }
            else
            {
                if (!std::isdigit(letter) && !(letter >= 'a' && letter <= 'f'))
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool eclIsValid(std::string_view value) const
    {
        if (value == "amb"
            || value == "blu"
            || value == "brn"
            || value == "gry"
            || value == "grn"
            || value == "hzl"
            || value == "oth"
            )
        {
            return true;
        }

        return false;
    }

    bool pidIsValid(std::string_view value) const
    {
        if (value.size() != 9)
        {
            return false;
        }

        for (const char& digit : value)
        {
            if (!std::isdigit(digit))
            {
                return false;
            }
        }

        return true;
    }

    // Doing some testing here to keep code repetition to a minimum.
    // Macros are definitevely evil, but they are powerful. In this case it allows us to do all checks for an entry in one line, with only specifying the name of the field once.
    // We can come close with templates, but we still need to type the field name 3 types: the string to check, the variable name and the correct validation fuction. This could probably be reduced to certain extend, but not as much as with macros since we don't have reflection for the name of the variable/validation function).
    void ParseInput(std::string_view inpuView)
    {
        // Template approach
        if (auto parseResult = ParseValue<unsigned int>(inpuView, "byr"))
        {
            byr |= byrIsValid(*parseResult);
        }

        if (auto parseResult = ParseValue<unsigned int>(inpuView, "iyr"))
        {
            iyr |= iyrIsValid(*parseResult);
        }

        if (auto parseResult = ParseValue<unsigned int>(inpuView, "eyr"))
        {
            eyr |= eyrIsValid(*parseResult);
        }

        // Macro approach
        CHECKANDVALIDATEINTSTRING(hgt);
        CHECKANDVALIDATESTRING(hcl);
        CHECKANDVALIDATESTRING(ecl);
        CHECKANDVALIDATESTRING(pid);
    }

    void Reset()
    {
        byr = iyr = eyr = hgt = hcl = ecl = pid = cid = false;
    }

    bool IsValid() const
    {
        return byr && iyr && eyr && hgt && hcl && ecl && pid; // && cid not needed
    }
};

int main()
{
    DocumentChecklist currentDoc;
    unsigned int validDocs = 0;
    for (std::string inputString; std::getline(std::cin, inputString);)
    {
        if (inputString == "")
        {
            if (currentDoc.IsValid())
            {
                ++validDocs;
            }
            currentDoc.Reset();
        }
        else
        {
            currentDoc.ParseInput(inputString);
        }
    }

    // Check last input
    if (currentDoc.IsValid())
    {
        ++validDocs;
    }

    std::cout << validDocs << "\n";
}