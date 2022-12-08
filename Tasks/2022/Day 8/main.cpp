#include <iostream>
#include <fstream>
#include <iostream>

constexpr static size_t sc_lineLen = 100;
static uint32_t s_forestSizeX = 0;
static uint32_t s_forestSizeY = 0;
static uint8_t** s_forest = nullptr;

static bool isVisible(uint32_t xT, uint32_t yT)
{
    if (xT == 0 || yT == 0 || xT == (s_forestSizeX - 1) || yT == (s_forestSizeY - 1))
        return true;
    
    uint8_t ownHeight = s_forest[xT][yT];

    uint8_t visibilityCount = 4;
    for (size_t x = 0 ; x < xT ; x++)
    {
        if (s_forest[x][yT] >= ownHeight)
        {
            visibilityCount--;
            break;
        }
    }
    for (size_t x = xT+1 ; x < s_forestSizeX ; x++)
    {
        if (s_forest[x][yT] >= ownHeight)
        {
            visibilityCount--;
            break;
        }
    }
    for (size_t y = 0 ; y < yT ; y++)
    {
        if (s_forest[xT][y] >= ownHeight)
        {
            visibilityCount--;
            break;
        }
    }
    for (size_t y = yT+1 ; y < s_forestSizeY ; y++)
    {
        if (s_forest[xT][y] >= ownHeight)
        {
            visibilityCount--;
            break;
        }
    }
    return (visibilityCount != 0);
}

uint32_t getScenicScore(uint32_t xT, uint32_t yT)
{
    if (xT == 0 || yT == 0 || xT == (s_forestSizeX - 1) || yT == (s_forestSizeY - 1))
        return 0;
    
    uint32_t ownHeight = s_forest[xT][yT];

    uint32_t scoreNorth = 0;
    uint32_t scoreEast = 0;
    uint32_t scoreSouth = 0;
    uint32_t scoreWest = 0;

    for (size_t y = yT ; y > 0 ; y--)
    {
        scoreNorth++;
        if (s_forest[xT][y-1] >= ownHeight)
            break;
    }
    for (size_t y = yT+1 ; y < s_forestSizeY ; y++)
    {
        scoreSouth++;
        if (s_forest[xT][y] >= ownHeight)
            break;
    }

    for (size_t x = xT ; x > 0 ; x--)
    {
        scoreWest++;
        if (s_forest[x-1][yT] >= ownHeight)
            break;
    }
    for (size_t x = xT+1 ; x < s_forestSizeX ; x++)
    {
        scoreEast++;
        if (s_forest[x][yT] >= ownHeight)
            break;
    }

    return (scoreNorth * scoreEast * scoreSouth * scoreWest);
}

int main(int argc, const char** argv)
{
    std::ifstream file("input.txt");

    if (!file)
        return EXIT_FAILURE;
    
    file.seekg(0, file.end);
    size_t fileSize = file.tellg();
    file.seekg(0, file.beg);

    bool initialised = false;
    size_t iY = 0;
    while (file)
    {
        std::string line(sc_lineLen, '\0');
        file.getline(line.data(), sc_lineLen, '\n');
        if (!initialised)
        {
            s_forestSizeX = std::strlen(line.c_str());
            s_forestSizeY = (fileSize / (s_forestSizeX + 1));
            
            std::cout << "The forest is " << s_forestSizeX << " x " << s_forestSizeY << " tiles in size!\n";

            s_forest = new uint8_t*[s_forestSizeX];
            for (size_t i = 0 ; i < s_forestSizeX ; i++)
                s_forest[i] = new uint8_t[s_forestSizeY] {0};
            
            initialised = true;
        }

        for (size_t iX = 0 ; iX < s_forestSizeX ; iX++)
        {
            uint8_t numVal = line[iX] - '0';
            s_forest[iX][iY] = numVal;
        }
        iY++;
    }

    uint32_t visibleTrees = 0;
    for (size_t x = 0 ; x < s_forestSizeX ; x++)
        for (size_t y = 0 ; y < s_forestSizeX ; y++)
            visibleTrees += isVisible(x,y);
    
    std::cout << "The sum of all visible trees is " << visibleTrees << '!' << std::endl;

    uint32_t bestScenicScore = 0;
    uint32_t bestX = 0;
    uint32_t bestY = 0;
    for (size_t x = 0 ; x < s_forestSizeX ; x++)
    {
        for (size_t y = 0 ; y < s_forestSizeX ; y++)
        {
            uint32_t scenicScore = getScenicScore(x, y);
            if (scenicScore > bestScenicScore)
            {
                bestScenicScore = scenicScore;
                bestX = x;
                bestY = y;
            }
        }
    }

    std::cout << "The best scenic score of the forest is " << bestScenicScore << " and it belongs to the tree at [" << (bestX+1) << ',' << (bestY+1) << "]!" << std::endl;

    for (size_t i = 0 ; i < s_forestSizeX ; i++)
        delete[] s_forest[i];
    delete[] s_forest;

    std::cout << "Done!" << std::endl;

    return EXIT_SUCCESS;
}