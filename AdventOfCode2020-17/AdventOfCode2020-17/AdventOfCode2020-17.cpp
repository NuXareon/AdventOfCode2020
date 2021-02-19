// AdventOfCode2020-17.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <optional>
#include <atomic>
#include <chrono>

/*
grid[w]             // Super-Plane
grid[w][z]          // PLane
grid[w][z][y]       // Line
grid[w][z][y][x]    // Object
*/
using Grid = std::vector<std::vector<std::vector<std::vector<bool>>>>;

void InitGrid(Grid& grid, size_t lineSize, unsigned int numIter)
{
    // Initialize the grid to the maximum possible space after at the end of iterations (valid grid positions grow on all sides after each iterations)
    // Beatiful, isn't it?
    grid = Grid(numIter*2 + 1, std::vector<std::vector<std::vector<bool>>>(numIter*2 + 1, std::vector<std::vector<bool>>(numIter*2+lineSize, std::vector<bool>(numIter*2 + lineSize, false))));
}

bool ProcessActive(const Grid& oldGrid, const int i, const int j, const int k, const int l)
{
    const bool cubeActive = oldGrid[i][j][k][l];
    unsigned int activeNeightbours = 0;

    for (auto x = i - 1; x <= i + 1; ++x)
    {
        if (x < 0 || x >= oldGrid.size())
        {
            continue;
        }

        for (auto y = j - 1; y <= j + 1; ++y)
        {
            if (y < 0 || y >= oldGrid[x].size())
            {
                continue;
            }

            for (auto z = k - 1; z <= k + 1; ++z)
            {
                if (z < 0 || z >= oldGrid[x][y].size())
                {
                    continue;
                }

                for (auto w = l - 1; w <= l + 1; ++w)
                {
                    if (w < 0 || w >= oldGrid[x][y][z].size())
                    {
                        continue;
                    }

                    if (x == i && y == j && z == k && w == l)
                    {
                        continue;
                    }

                    if (oldGrid[x][y][z][w])
                    {
                        ++activeNeightbours;
                    }

                    if (activeNeightbours > 3)
                    {
                        return false;
                    }
                }
            }
        }
    }

    if (cubeActive)
    {
        return activeNeightbours == 2 || activeNeightbours == 3;
    }

    return activeNeightbours == 3;
}

std::pair<Grid, size_t> ReadInput(const unsigned int numIter)
{
    Grid grid;
    bool initialized = false;

    size_t initialRowSize = 0;
    size_t rowIndex = numIter;
    for (std::string input; std::getline(std::cin, input);)
    {
        if (!initialized)
        {
            initialRowSize = input.size();
            InitGrid(grid, input.size(), numIter);
            initialized = true;
        }

        auto& superPlane = grid[numIter];
        auto& plane = superPlane[numIter];
        auto& row = plane[rowIndex];

        size_t objectIndex = numIter;
        for (const char c : input)
        {
            if (c == '#')
            {
                row[objectIndex] = true;
            }
            ++objectIndex;
        }
        ++rowIndex;
    }

    return { grid, initialRowSize };
}

Grid ProcessSingleThread(Grid grid, const unsigned int numIterations, const size_t initialRowSize)
{
    Grid oldGrid = grid;

    for (int n = 0; n < numIterations; ++n)
    {
        std::swap(oldGrid, grid);

        for (unsigned int i = numIterations - n - 1; i <= numIterations + n + 1; ++i)
        {
            auto& superPlane = grid[i];
            for (unsigned int j = numIterations - n - 1; j <= numIterations + n + 1; ++j)
            {
                auto& plane = superPlane[j];
                for (unsigned int k = numIterations - n - 1; k < numIterations + n + initialRowSize + 1; ++k)
                {
                    auto& row = plane[k];
                    for (unsigned int l = numIterations - n - 1; l < numIterations + n + initialRowSize + 1; ++l)
                    {
                        row[l] = ProcessActive(oldGrid, i, j, k, l);
                    }
                }
            }
        }
    }

    return grid;
}

// Initial implementation, creating one thread per plane needing to be process.
Grid ProcessMultiThreads(Grid grid, const unsigned int numIterations, const size_t initialRowSize)
{
    Grid oldGrid = grid;

    for (int n = 0; n < numIterations; ++n)
    {
        std::vector<std::thread> threads;

        std::swap(oldGrid, grid);

        for (unsigned int i = numIterations - n - 1; i <= numIterations + n + 1; ++i)
        {
            auto& superPlane = grid[i];
            for (unsigned int j = numIterations - n - 1; j <= numIterations + n + 1; ++j)
            {
                threads.emplace_back([&grid, &oldGrid, i, j, n, initialRowSize, numIterations]()
                    {
                        auto& plane = grid[i][j];
                        for (unsigned int k = numIterations - n - 1; k < numIterations + n + initialRowSize + 1; ++k)
                        {
                            auto& row = plane[k];
                            for (unsigned int l = numIterations - n - 1; l < numIterations + n + initialRowSize + 1; ++l)
                            {
                                row[l] = ProcessActive(oldGrid, i, j, k, l);
                            }
                        }
                    });

            }
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }

    return grid;
}

// Use a list of coordinates to know whish planes need processing. 
// Use a mutex to guarantee access and modification to this shared list is protected.
Grid ProcessFixedThreads(Grid grid, const unsigned int numIterations, const unsigned int numThreads, const size_t initialRowSize)
{
    std::mutex mutex;
    Grid oldGrid = grid;

    for (int n = 0; n < numIterations; ++n)
    {
        std::vector<std::thread> threads;

        std::swap(oldGrid, grid);

        std::vector<std::pair<unsigned int, unsigned int>> planeCoords;
        planeCoords.reserve(((numIterations + n + 1) - (numIterations - n - 1)) * ((numIterations + n + 1) - (numIterations - n - 1)));

        for (unsigned int i = numIterations - n - 1; i <= numIterations + n + 1; ++i)
        {
            for (unsigned int j = numIterations - n - 1; j <= numIterations + n + 1; ++j)
            {
                // Technically, we could optimize this, so instead of regenerating the fill index list, we only add the new possible indices every iterations.
                planeCoords.emplace_back(i, j);
            }
        }

        for (unsigned int i = 0; i < numThreads; ++i)
        {
            threads.emplace_back([&grid, &oldGrid, &planeCoords, n, numIterations, initialRowSize, &mutex]()
                {
                    for (;;)
                    {
                        const auto planeCoord = [&planeCoords, &mutex]() -> std::optional<std::pair<unsigned int, unsigned int>>
                        {
                            std::scoped_lock lock(mutex);
                            if (planeCoords.size() == 0)
                            {
                                return std::nullopt;
                            }

                            const auto planeCoord = planeCoords.back();
                            planeCoords.pop_back();
                            return planeCoord;
                        }();

                        if (!planeCoord)
                        {
                            return;
                        }

                        auto& plane = grid[planeCoord->first][planeCoord->second];
                        for (unsigned int k = numIterations - n - 1; k < numIterations + n + initialRowSize + 1; ++k)
                        {
                            auto& row = plane[k];
                            for (unsigned int l = numIterations - n - 1; l < numIterations + n + initialRowSize + 1; ++l)
                            {
                                row[l] = ProcessActive(oldGrid, planeCoord->first, planeCoord->second, k, l);
                            }
                        }
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }

    return grid;
}

// Use an atomic index variable to indicate which planes are left to process
Grid ProcessAtomic(Grid grid, const unsigned int numIterations, const unsigned int numThreads, const size_t initialRowSize)
{
    Grid oldGrid = grid;

    for (int n = 0; n < numIterations; ++n)
    {
        std::vector<std::thread> threads;

        const unsigned int planeSize = (numIterations + n + 1) - (numIterations - n - 1) + 1;
        const unsigned int maxIdx = ((numIterations + n + 1) - (numIterations - n - 1) + 1) * ((numIterations + n + 1) - (numIterations - n - 1) + 1);

        std::atomic<unsigned int> atomicIdx = 0;

        std::swap(oldGrid, grid);

        for (unsigned int i = 0; i < numThreads; ++i)
        {
            threads.emplace_back([&grid, &oldGrid, n, planeSize, maxIdx, initialRowSize, numIterations, &atomicIdx]()
                {
                    for (;;)
                    {
                        const unsigned int index = atomicIdx.fetch_add(1, std::memory_order_relaxed);

                        if (index >= maxIdx)
                        {
                            return;
                        }

                        const unsigned int i = (index / planeSize) + numIterations - n - 1;
                        const unsigned int j = (index % planeSize) + numIterations - n - 1;

                        auto& plane = grid[i][j];
                        for (unsigned int k = numIterations - n - 1; k < numIterations + n + initialRowSize + 1; ++k)
                        {
                            auto& row = plane[k];
                            for (unsigned int l = numIterations - n - 1; l < numIterations + n + initialRowSize + 1; ++l)
                            {
                                row[l] = ProcessActive(oldGrid, i, j, k, l);
                            }
                        }
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }

    return grid;
}

// Assign an index range for each thread.
Grid ProcessNoSynch(Grid grid, const unsigned int numIterations, const unsigned int numThreads, const size_t initialRowSize)
{
    Grid oldGrid = grid;

    for (int n = 0; n < numIterations; ++n)
    {
        std::vector<std::thread> threads;

        const unsigned int planeSize = (numIterations + n + 1) - (numIterations - n - 1) + 1;
        const unsigned int maxIdx = ((numIterations + n + 1) - (numIterations - n - 1) + 1) * ((numIterations + n + 1) - (numIterations - n - 1) + 1);
        const unsigned int itemsPerThread = maxIdx / numThreads;

        std::swap(oldGrid, grid);

        for (int currentMin = 0; currentMin < maxIdx; currentMin += itemsPerThread)
        {
            const unsigned int currentMax = currentMin + itemsPerThread;

            threads.emplace_back([&grid, &oldGrid, n, planeSize, maxIdx, initialRowSize, numIterations, currentMin, currentMax]()
                {
                    for (unsigned int index = currentMin; index < currentMax; ++index)
                    {
                        if (index >= maxIdx)
                        {
                            return;
                        }

                        const unsigned int i = (index / planeSize) + numIterations - n - 1;
                        const unsigned int j = (index % planeSize) + numIterations - n - 1;

                        auto& plane = grid[i][j];
                        for (unsigned int k = numIterations - n - 1; k < numIterations + n + initialRowSize + 1; ++k)
                        {
                            auto& row = plane[k];
                            for (unsigned int l = numIterations - n - 1; l < numIterations + n + initialRowSize + 1; ++l)
                            {
                                row[l] = ProcessActive(oldGrid, i, j, k, l);
                            }
                        }
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }

    return grid;
}

unsigned int CountActiveCubes(const Grid& grid)
{
    unsigned int activeCubes = 0;

    for (const auto& superPlane : grid)
    {
        for (const auto& plane : superPlane)
        {
            for (const auto& row : plane)
            {
                for (const bool cubeActive : row)
                {
                    if (cubeActive)
                    {
                        ++activeCubes;
                    }
                }
            }
        }
    }

    return activeCubes;
}

int main()
{
    // n = number of iterations
    // grid dimension : (2*n+w) x (2*n+z) + (2*n+y) + (2*n+x)
    // for w = 1 and z = 1
    // x and y dependant on input size

    constexpr unsigned int numIterations = 6;
    constexpr unsigned int numThreads = 8;

    auto [grid, initialRowSize] = ReadInput(numIterations);

    const auto beforeSingleThread = std::chrono::high_resolution_clock::now();
    const Grid singleThreadGrid = ProcessSingleThread(grid, numIterations, initialRowSize);
    const auto afterSingleThread = std::chrono::high_resolution_clock::now();
    const Grid multiThreadsGrid = ProcessMultiThreads(grid, numIterations, initialRowSize);
    const auto afterMultiThread = std::chrono::high_resolution_clock::now();
    const Grid fixedThreadsGrid = ProcessFixedThreads(grid, numIterations, numThreads, initialRowSize);
    const auto afterFixedThread = std::chrono::high_resolution_clock::now();
    const Grid atomicGrid = ProcessAtomic(grid, numIterations, numThreads, initialRowSize);
    const auto afterAtomic = std::chrono::high_resolution_clock::now();
    const Grid noSynchGrid = ProcessNoSynch(grid, numIterations, numThreads, initialRowSize);
    const auto afterNoSynch = std::chrono::high_resolution_clock::now();

    const unsigned int singleThreadActiveCubes = CountActiveCubes(singleThreadGrid);
    const unsigned int multiThreadActiveCubes = CountActiveCubes(multiThreadsGrid);
    const unsigned int fixedThreadActiveCubes = CountActiveCubes(fixedThreadsGrid);
    const unsigned int atomicActiveCubes = CountActiveCubes(atomicGrid);
    const unsigned int noSynchActiveCubes = CountActiveCubes(noSynchGrid);

    std::cout << singleThreadActiveCubes << ". Took: " << std::chrono::duration_cast<std::chrono::milliseconds>(afterSingleThread - beforeSingleThread).count() << "\n";
    std::cout << multiThreadActiveCubes << ". Took: " << std::chrono::duration_cast<std::chrono::milliseconds>(afterMultiThread - afterSingleThread).count() << "\n";
    std::cout << fixedThreadActiveCubes << ". Took: " << std::chrono::duration_cast<std::chrono::milliseconds>(afterFixedThread - afterMultiThread).count() << "\n";
    std::cout << atomicActiveCubes << ". Took: " << std::chrono::duration_cast<std::chrono::milliseconds>(afterAtomic - afterFixedThread).count() << "\n";
    std::cout << noSynchActiveCubes << ". Took: " << std::chrono::duration_cast<std::chrono::milliseconds>(afterNoSynch - afterAtomic).count() << "\n";
}
