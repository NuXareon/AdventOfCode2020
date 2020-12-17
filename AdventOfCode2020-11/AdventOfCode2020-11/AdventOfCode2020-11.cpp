// AdventOfCode2020-11.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <thread>
#include <barrier>
#include <latch>
#include <algorithm>

bool CheckDirectionEmpty(const std::vector<std::string>& map, unsigned int i, unsigned int j, std::pair<unsigned int, unsigned int> direction)
{
    for (i += direction.first, j += direction.second; i < map.size() && i >= 0 && j < map[i].size() && j >= 0; i += direction.first, j += direction.second)
    {
        if (map[i][j] == 'L')
        {
            return true;
        }
        if (map[i][j] == '#')
        {
            return false;
        }
    }

    return true;
}

std::string ProcessLine(const std::vector<std::string>& map, unsigned int i)
{
    std::string_view line(map[i]);
    std::string newLine;
    for (unsigned int j = 0; j < line.size(); ++j)
    {
        // Check all directions
        char cell = line[j];
        if (cell == 'L')
        {
            bool isAdjacentEmpty = CheckDirectionEmpty(map, i, j, { 1,0 });
            if (isAdjacentEmpty)
            {
                isAdjacentEmpty &= CheckDirectionEmpty(map, i, j, { 1,1 });
            }
            if (isAdjacentEmpty)
            {
                isAdjacentEmpty &= CheckDirectionEmpty(map, i, j, { 0,1 });
            }
            if (isAdjacentEmpty)
            {
                isAdjacentEmpty &= CheckDirectionEmpty(map, i, j, { -1,1 });
            }
            if (isAdjacentEmpty)
            {
                isAdjacentEmpty &= CheckDirectionEmpty(map, i, j, { -1,0 });
            }
            if (isAdjacentEmpty)
            {
                isAdjacentEmpty &= CheckDirectionEmpty(map, i, j, { -1,-1 });
            }
            if (isAdjacentEmpty)
            {
                isAdjacentEmpty &= CheckDirectionEmpty(map, i, j, { 0,-1 });
            }
            if (isAdjacentEmpty)
            {
                isAdjacentEmpty &= CheckDirectionEmpty(map, i, j, { 1,-1 });
            }

            if (isAdjacentEmpty)
            {
                cell = '#';
            }
        }
        else if (cell == '#')
        {
            unsigned int occupied = 0;

            bool isAdjacentEmpty = CheckDirectionEmpty(map, i, j, { 1,0 });
            if (!CheckDirectionEmpty(map, i, j, { 1,0 }))
            {
                ++occupied;
            }
            if (!CheckDirectionEmpty(map, i, j, { 1,1 }))
            {
                ++occupied;
            }
            if (!CheckDirectionEmpty(map, i, j, { 0,1 }))
            {
                ++occupied;
            }
            if (!CheckDirectionEmpty(map, i, j, { -1,1 }))
            {
                ++occupied;
            }
            if (!CheckDirectionEmpty(map, i, j, { -1,0 }))
            {
                ++occupied;
            }
            if (occupied <= 4 && !CheckDirectionEmpty(map, i, j, { -1,-1 }))
            {
                ++occupied;
            }
            if (occupied <= 4 && !CheckDirectionEmpty(map, i, j, { 0,-1 }))
            {
                ++occupied;
            }
            if (occupied <= 4 && !CheckDirectionEmpty(map, i, j, { 1,-1 }))
            {
                ++occupied;
            }

            if (occupied > 4)
            {
                cell = 'L';
            }
        }

        newLine.push_back(cell);
    }

    return newLine;
}

int main()
{
    std::vector<std::string> map;

    for (std::string line; std::getline(std::cin, line);)
    {
        map.emplace_back(std::move(line));
    }

    // Generate 8 threads
    const unsigned int numElemsPerThread = static_cast<unsigned int>(std::ceil(map.size()/8.0f));
   
    unsigned int occupiedSeats = 0;
    std::vector<std::string> newMap(map.size());

    // Latch used to stop main thread until processing is finished
    std::latch waitForResult(1);

    // Barrier used to synchronize the threads each iteration and check for the result
    std::barrier barrier(static_cast<ptrdiff_t>(std::ceil(static_cast<float>(map.size())/static_cast<float>(numElemsPerThread))), 
        [&map, &newMap, &occupiedSeats, &waitForResult]() noexcept
        {
            if (newMap == map)
            {
                for (std::string_view line : map)
                {
                    for (const char cell : line)
                    {
                        if (cell == '#')
                        {
                            ++occupiedSeats;
                        }
                    }
                }

                waitForResult.count_down();
            }
            
            std::swap(map, newMap);   
        });
    
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < map.size(); i += numElemsPerThread)
    {
        std::thread thread = std::thread([&waitForResult, &barrier, &map, &newMap, min = i, max = std::min(i + numElemsPerThread, map.size())]
            {
                // Stop processing if the lacth has been cleared. We want to prevent threads from processesing before main thread resumes.
                for (;!waitForResult.try_wait();)
                {
                    for (unsigned int j = min; j < max; ++j)
                    {
                        // No read lock needed since map is never modified during thread processing
                        std::string newLine = ProcessLine(map, j);
                        newMap[j] = std::move(newLine); // No write lock needed since threads write different lines
                    }
                    barrier.arrive_and_wait();
                }
            });

        threads.push_back(std::move(thread));
    }

    waitForResult.wait();

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << occupiedSeats << "\n";
}