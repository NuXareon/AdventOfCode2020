// AdventOfCode2020-14.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <unordered_map>
#include <locale>
#include <vector>

using MemoryPool = std::unordered_map<unsigned long long, unsigned long long>;

struct bracket_is_space : std::ctype<char> 
{
    bracket_is_space(std::size_t refs = 0) : ctype(make_table(), false, refs) {}
    static const mask* make_table()
    {
        // make a copy of the "C" locale table
        static std::vector<mask> v(classic_table(), classic_table() + table_size);
        v['['] |= space;
        v[']'] |= space;
        return &v[0];
    }
};

// Note that copying of floatingBits is intentional, since we need to modify it for each recursive call while keeping the original
void ProcessFloatingMask(MemoryPool& mem, const unsigned long long value, unsigned long long address, std::vector<size_t> floatingBits)
{
    // Base case (N == 0)
    if (floatingBits.empty())
    {
        mem[address] = value;
        return;
    }

    // Extract index from mask
    const auto index = floatingBits.back();
    floatingBits.pop_back();    // N-1

    // Set index to 0
    unsigned long long mask = -1;
    address &= mask - (1ull << index);    // mask: 111...101...111
    ProcessFloatingMask(mem, value, address, floatingBits);

    // Set index to 1
    address |= 1ull << index;
    ProcessFloatingMask(mem, value, address, floatingBits);
}

int main()
{
    unsigned long long onesMask = 0;    // 000...000
    std::vector<size_t> floatingBits;
    MemoryPool mem;

    std::cin.imbue(std::locale(std::cin.getloc(), new bracket_is_space));

    for (std::string instruction; std::cin >> instruction;)
    {
        if (instruction == "mask")
        {
            std::string equals;
            std::cin >> equals;

            std::string mask;
            std::cin >> mask;

            onesMask = 0;    // 000...000
            floatingBits.clear();

            size_t index = mask.size() - 1;
            for (const char& digit : mask)
            {
                if (digit == '1')
                {
                    onesMask |= 1ull << index;
                }
                else if (digit == '0')
                {
                }
                else if (digit == 'X')
                {
                    floatingBits.push_back(index);
                }
                --index;
            }
        }
        else if (instruction == "mem")
        {
            unsigned long long address;
            std::cin >> address;

            std::string equals;
            std::cin >> equals;
            
            unsigned long long value;
            std::cin >> value;

            address |= onesMask;
            ProcessFloatingMask(mem, value, address, floatingBits);
        }
    }

    unsigned long long result = 0;
    for (auto& node : mem)
    {
        result += node.second;
    }

    std::cout << result << "\n";
}