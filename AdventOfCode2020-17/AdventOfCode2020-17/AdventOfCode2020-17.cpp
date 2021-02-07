// AdventOfCode2020-17.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>

// TODO
// Algo:
// A - check neightbours for each cube: 26 * N
// B - count active neightbours and check totals : N + 26 * a + N = 2N + 26 * a
// Multithread!

/*
grid[w]             // Super-Plane
grid[w][z]          // PLane
grid[w][z][y]       // Line
grid[w][z][y][x]    // Object
*/
using Grid = std::vector<std::vector<std::vector<std::vector<bool>>>>;
/*
void IncreaseGridSize(Grid& grid)
{
    for (int i = 0; i < grid.size(); ++i)
    {
        auto& superPlane = grid[i];
        for (int j = 0; j < superPlane.size(); ++j)
        {
            auto& plane = superPlane[j];
            for (int k = 0; k < plane.size(); ++k)
            {
                auto& row = plane[k];
                row.insert(row.begin(), false);
                row.insert(row.end(), false);
            }
            plane.emplace(plane.begin(), plane[0].size(), false);
            plane.emplace(plane.end(), plane[0].size(), false);
        }
        superPlane.emplace(superPlane.begin(), superPlane[0].size(), std::vector<bool>(superPlane[0][0].size(), false));
        superPlane.emplace(superPlane.end(), superPlane[0].size(), std::vector<bool>(superPlane[0][0].size(), false));
    }
    grid.emplace(grid.begin(), grid[0].size(), std::vector<std::vector<bool>>(grid[0][0].size(), std::vector<bool>(grid[0][0][0].size(), false)));
    grid.emplace(grid.end(), grid[0].size(), std::vector<std::vector<bool>>(grid[0][0].size(), std::vector<bool>(grid[0][0][0].size(), false)));
}
*/
void InitGrid(Grid& grid, size_t lineSize, unsigned int numIter)
{
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

int main()
{
    // n = number of iterations
    // grid dimension : (2*n+w) x (2*n+z) + (2*n+y) + (2*n+x)
    // for w = 1 and z = 1
    // x and y dependant on input size

    constexpr unsigned int numIterations = 6;

    Grid grid;
    bool initialized = false;

    size_t initialRowSize = 0;
    size_t rowIndex = numIterations;
    for (std::string input; std::getline(std::cin, input);)
    {
        if (!initialized)
        {
            initialRowSize = input.size();
            InitGrid(grid, input.size(), numIterations);
            initialized = true;
        }

        auto& superPlane = grid[numIterations];
        auto& plane = superPlane[numIterations];
        auto& row = plane[rowIndex];

        size_t objectIndex = numIterations;
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

    Grid oldGrid = grid;

    // At the start of the iterations swap new and old grids
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

    std::cout << activeCubes << "\n";
}
