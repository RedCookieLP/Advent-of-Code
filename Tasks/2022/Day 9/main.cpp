#include <iostream>
#include <fstream>
#include <string>

constexpr size_t sc_lineLen = 10;

struct Vector2
{
    int x;
    int y;
};

constexpr static size_t sc_boardSize = 1000;
static size_t s_visitedTiles = 0;
static bool* s_board = nullptr;

static void visitTile(const Vector2& pos)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= sc_boardSize || pos.y >= sc_boardSize)
        return;
    
    bool& tile = s_board[pos.x + (pos.y * sc_boardSize)];
    //  std::cout << "\tHas tile at [" << pos.x << ',' << pos.y << "] been visited ? " << (tile ? "yes" : "no") << std::endl;

    if (tile)
        return;
    tile = true;
    s_visitedTiles++;
}

static void pullRope(const Vector2& head, Vector2& tail, size_t tailIndex)
{
    Vector2 dif = Vector2{head.x - tail.x, head.y - tail.y};

    if (dif.x <= 1 && dif.x >= -1 && dif.y <= 1 && dif.y >= -1)
        return;
    
    Vector2 movement = {0,0};

    if (dif.x != 0)
        movement.x += (dif.x / abs(dif.x));
    if (dif.y != 0)
        movement.y += (dif.y / abs(dif.y));
    
    //  std::cout << "\t\tPulling knot #" << tailIndex << " from [" << tail.x << ',' << tail.y << "] to head [" << head.x << ',' << head.y << "] (knot #" << (tailIndex - 1) << ") --> movement: [" << movement.x << ',' << movement.y << ']' << std::endl;

    tail.x += movement.x;
    tail.y += movement.y;
}

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,

    UNKNOWN
};

constexpr static size_t sc_ropeLength = 10;
static Vector2 s_rope[sc_ropeLength];

void processSteps(Direction dir, size_t stepCount)
{
    Vector2& head = s_rope[0];
    Vector2& tail = s_rope[sc_ropeLength - 1];

    //  std::cout << "Processing direction \"" << (dir == UP ? "UP" : (dir == DOWN ? "DOWN" : (dir == LEFT ? "LEFT" : (dir == RIGHT ? "RIGHT" : "UNKNOWN"))))
    //            << "\" with " << stepCount << " step(s). Currently at [" << tail.x << ',' << tail.y << ']' << std::endl;

    while (stepCount > 0)
    {
        stepCount--;

        head.x += (dir == LEFT ? -1 : (dir == RIGHT ? 1 : 0));
        head.y += (dir == DOWN ? -1 : (dir == UP ? 1 : 0));

        for (size_t i = 1 ; i < sc_ropeLength ; i++)
            pullRope(s_rope[i-1], s_rope[i], i);

        visitTile(tail);
    }
}

int main(int argc, const char** argv)
{
    std::ifstream file("input.txt");
    if (!file)
        return EXIT_FAILURE;
    
    s_board = new bool[sc_boardSize * sc_boardSize] {false};
    for (size_t i = 0 ; i < sc_ropeLength ; i++)
        s_rope[i] = Vector2{500, 500};
    visitTile(s_rope[sc_ropeLength-1]);

    while (file)
    {
        std::string line(sc_lineLen, '\0');
        file.getline(line.data(), sc_lineLen, '\n');
        size_t lineLen = std::strlen(line.c_str());
        if (lineLen == 0)
            continue;
        
        char dir = line[0];
        Direction direction = (dir == 'U' ? UP : (dir == 'D' ? DOWN : (dir == 'L' ? LEFT : (dir == 'R' ? RIGHT : UNKNOWN))));

        size_t stepCount = std::atoll(line.c_str() + 2ULL);

        processSteps(direction, stepCount);
    }

    delete[] s_board;

    std::cout << "Through that whole journey, T visited " << s_visitedTiles << " tiles!" << std::endl;

    file.close();

    return EXIT_SUCCESS;
}