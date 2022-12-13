#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class PathElement
{
public:
    PathElement()
        : m_x(0), m_y(0), m_elevation(0)
    {
        memset(m_neighbours, NULL, sizeof(PathElement*) * 4u);
    }
    PathElement(uint8_t xPos, uint8_t yPos, uint8_t elevation)
        : m_x(xPos), m_y(yPos), m_elevation(elevation)
    {
        memset(m_neighbours, NULL, sizeof(PathElement*) * 4u);
    }
    PathElement(const PathElement&) = default;
    PathElement& operator=(const PathElement&) = default;
    PathElement(PathElement&&) = default;
    PathElement& operator=(PathElement&&) = default;
public:
    bool operator==(const PathElement& o) const { return (m_x == o.m_x && m_y == o.m_y); }
    bool operator!=(const PathElement& o) const { return !(*this == o); }
public:
    void addNeighbour(PathElement* n)
    {
        if (!n || hasAsNeighbour(n))
            return;
        
        for (size_t i = 0 ; i < 4 ; i++)
        {
            if (!m_neighbours[i])
            {
                m_neighbours[i] = n;
                return;
            }
        }
    }
    bool hasAsNeighbour(const PathElement* const elem)
    {
        if (!elem)
            return false;
        
        for (size_t i = 0 ; i < 4 ; i++)
        {
            const PathElement* n = m_neighbours[i];
            if (n && *n == *elem)
                return true;
        }
        return false;
    }
    PathElement** getNeighbours()
    {
        return m_neighbours;
    }
    uint8_t x() const { return m_x; }
    uint8_t y() const { return m_y; }
    uint8_t elevation() const { return m_elevation; }
private:
    uint8_t m_x;
    uint8_t m_y;
    uint8_t m_elevation;
    PathElement* m_neighbours[4];
};

template <typename T>
constexpr T dif(T a, T b) { return (a < b ? b-a : a-b); }

constexpr static size_t sc_lineLen = 150;
static PathElement** s_neighbourMap = nullptr;
static size_t s_rows = 0;
static size_t s_columns = 0;

void buildPath()
{
    for (size_t y = 0 ; y < s_rows ; y++)
    {
        for (size_t x = 0 ; x < s_columns ; x++)
        {
            PathElement& current = s_neighbourMap[y][x];

            PathElement* nLeft = (x > 0 ? &s_neighbourMap[y][x-1] : nullptr);
            PathElement* nRight = (x < s_columns-1 ? &s_neighbourMap[y][x+1] : nullptr);
            PathElement* nTop = (y > 0 ? &s_neighbourMap[y-1][x] : nullptr);
            PathElement* nBottom = (y < s_rows-1 ? &s_neighbourMap[y+1][x] : nullptr);

            if (nLeft && dif(current.elevation(), nLeft->elevation()) <= 1)
                current.addNeighbour(nLeft);
            if (nTop && dif(current.elevation(), nTop->elevation()) <= 1)
                current.addNeighbour(nTop);
            if (nRight && dif(current.elevation(), nRight->elevation()) <= 1)
                current.addNeighbour(nRight);
            if (nBottom && dif(current.elevation(), nBottom->elevation()) <= 1)
                current.addNeighbour(nBottom);
        }
    }
}

void solveForShortestPath(std::vector<PathElement>& finalPath)
{
    std::vector<PathElement> currentPath;
    size_t shortestPath = size_t(-1);
}

int main(int argc, const char** argv)
{
    std::ifstream file("input.txt");
    
    file.seekg(0,file.end);
    size_t fileSize = file.tellg();
    file.seekg(0,file.beg);

    PathElement* startPos;
    PathElement* endPos;

    size_t r = 0;
    while (file)
    {
        std::string line(sc_lineLen,'\0');
        file.getline(line.data(), sc_lineLen);
        const size_t lineLen = std::strlen(line.c_str());
        if (lineLen == 0)
            continue;
        
        if (!s_neighbourMap)
        {
            s_rows = fileSize / (lineLen + 1);
            s_columns = lineLen;
            std::cout << "The map is " << s_columns << " by " << s_rows << " in size!" << std::endl;
            
            s_neighbourMap = new PathElement*[s_rows];
            for (size_t i = 0 ; i < s_rows ; i++)
                s_neighbourMap[i] = new PathElement[s_columns];
        }

        for (size_t c = 0 ; c < s_columns ; c++)
        {
            PathElement& elem = s_neighbourMap[r][c];
            char ch = line[c];
            if (!(ch == 'S' || ch == 'E'))
                elem = PathElement(c,r,uint8_t(ch - 'a'));
            else
            {
                if (ch == 'S')
                {
                    elem = PathElement(c,r,0);
                    startPos = &elem;
                }
                else
                {
                    elem = PathElement(c,r,25);
                    endPos = &elem;
                }
            }
        }
        r++;
    }
    file.close();

    std::cout << "The complete heightmap is " << s_columns << "x" << s_rows << " in size!" << std::endl;

    std::cout << "Building paths..." << std::endl;
    buildPath();
    std::cout << "Done!" << std::endl;

//    for (size_t y = 0 ; y < s_rows ; y++)
//    {
//        for (size_t x = 0 ; x < s_columns ; x++)
//            std::cout << char('a' + s_heightMap[y][x]);
//        std::cout << std::endl;;
//    }

    for (size_t i = 0 ; i < s_rows ; i++)
        delete[] s_neighbourMap[i];
    delete[] s_neighbourMap;

    return EXIT_SUCCESS;
}