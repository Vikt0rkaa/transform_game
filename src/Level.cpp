#include "Level.hpp"

// Map legend (19x13):
//  '#' = Wall
//  '.' = Empty floor
//  'G' = Glass (Blue only)
//  'B' = Block (pushable by Red)
//  'X' = Goal

//LEVEL 1
static const char* MAP1[ROWS] = {
    "###################",
    "#...#.....#.......#",
    "#.#.#S#.#.#.#####.#",
    "#.#.B.#B....#.....#",
    "#.#####.#####.###.#",
    "#.....#.#.G.#.#B..#",
    "#.#####.#.#.#.#.###",
    "#.#.GGG.#.#...#...#",
    "#.#####G#.#####.#.#",
    "#.#....B..G.....#.#",
    "#.####.##.#.###.#.#",
    "#......##.#.#.X...#",
    "###################"
};

//LEVEL 2
static const char* MAP2[ROWS] = {
    "###################",
    "#..B..#####X.GGG.##",
    "####.#####.#.###.##",
    "#....GGGG..B.#GGG##",
    "#G###.#####G##.##.#",
    "#B.G#.##..B.#.....#",
    "#.#G#...#.#######B#",
    "###G#GGG#.B...GG..#",
    "###B#######.#.###G#",
    "#.........#.B..G..#",
    "#.#.#####.#.#####.#",
    "#..#....#...G.....#",
    "###################"
};

//LEVEL 3
static const char* MAP3[ROWS] = {
    "###################",
    "#B.###.....###....#",
    "#..#G#.###.#G#.##.#",
    "#.##.#.#B#.#..##..#",
    "#....#.#.#.######.#",
    "#.##.#.#.#.#.GGG#.#",
    "#.#G##.#.#.#.#.##.#",
    "#.#G#..#.#...#....#",
    "#.#G####.#####.##.#",
    "#.#......#.....#..#",
    "#.######.#.###.##.#",
    "#........#.#B.GGX.#",
    "###################"
};

static const char** MAPS[3] = {
    (const char**)MAP1,
    (const char**)MAP2,
    (const char**)MAP3
};

Level::Level() 
{
    buildMap();
}

void Level::buildMap() {
    const char** map = MAPS[currentLevel];
    for (int r = 0; r < ROWS; ++r) 
    {
        for (int c = 0; c < COLS; ++c) 
        {
            char ch = map[r][c];
            if (ch == '#') grid[r][c] = Tile::Wall;
            else if (ch == 'G') grid[r][c] = Tile::Glass;
            else if (ch == 'B') grid[r][c] = Tile::Block;
            else if (ch == 'X') grid[r][c] = Tile::Goal;
            else if (ch == 'S')grid[r][c] = Tile::Star;
            else grid[r][c] = Tile::Empty;
        }
        starCollected = false;
    }
}

void Level::reset() 
{
    buildMap();
}

void Level::nextLevel() 
{
    if (currentLevel < 2)
        ++currentLevel;
    buildMap();
}

bool Level::isFinalLevel() const 
{
    return currentLevel == 2;
}

int Level::getCurrentLevel() const 
{
    return currentLevel;
}

bool Level::isStarCollected() const
{
    return starCollected;
}

Tile Level::getTile(int c, int r) const 
{
    if (c < 0 || c >= COLS || r < 0 || r >= ROWS)
        return Tile::Wall;
    return grid[r][c];
}

void Level::setTile(int c, int r, Tile t) 
{
    if (c < 0 || c >= COLS || r < 0 || r >= ROWS)
        return;
    grid[r][c] = t;
}

bool Level::isSolid(int c, int r) const 
{ auto t = getTile(c,r); return t == Tile::Wall || t == Tile::Block; }
bool Level::isGlass(int c, int r) const 
{ return getTile(c,r) == Tile::Glass; }
bool Level::isBlock(int c, int r) const 
{ return getTile(c,r) == Tile::Block; }
bool Level::isGoal(int c, int r) const 
{ return getTile(c,r) == Tile::Goal;  }

bool Level::tryCollectStar(int c, int r)
{
    if (getTile(c, r) == Tile::Star)
    {
        setTile(c, r, Tile::Empty);
        starCollected = true;
        return true;
    }
    return false;
}

void Level::draw(sf::RenderWindow& window, float time, sf::Font& font)
{
    sf::RectangleShape tile(sf::Vector2f(TILE - 1.0f, TILE - 1.0f));

    for (int r = 0; r < ROWS; ++r) 
    {
        for (int c = 0; c < COLS; ++c) 
        {
            tile.setPosition(c * TILE + 0.5f, r * TILE + 0.5f);
            tile.setOutlineThickness(0);

            switch (grid[r][c]) 
            {
                case Tile::Wall:
                {
                    tile.setFillColor(sf::Color(55, 55, 78));
                    tile.setOutlineColor(sf::Color(90, 90, 120));
                    tile.setOutlineThickness(1.0f);
                }
                break;
                case Tile::Glass:
                {
                    tile.setFillColor(sf::Color(140, 210, 255, 160));
                    tile.setOutlineColor(sf::Color(180, 230, 255, 220));
                    tile.setOutlineThickness(1.5f);
                }
                break;
                case Tile::Block:
                {
                    tile.setFillColor(sf::Color(160, 100, 40));
                    tile.setOutlineColor(sf::Color(110, 65, 20));
                    tile.setOutlineThickness(1.5f);
                }
                break;
                case Tile::Goal:
                {
                    float pulse = 180.0f + 75.0f * std::sin(time * 3.0f);
                    tile.setFillColor(sf::Color(255, (sf::Uint8)pulse, 0));
                    tile.setOutlineColor(sf::Color(255, 220, 0));
                    tile.setOutlineThickness(2.0f);
                }
                break;
                case Tile::Star:
                {
                    tile.setFillColor(sf::Color(60, 0, 100));
                    tile.setOutlineColor(sf::Color(180, 80, 255));
                    tile.setOutlineThickness(2.0f);
                    window.draw(tile);

                    sf::Text star("*", font, 28);
                    float bob = 6.0f * std::sin(time * 4.0f);
                    star.setPosition(
                        c * TILE + TILE / 2.f - star.getLocalBounds().width / 2.0f,
                        r * TILE + TILE / 2.f - star.getLocalBounds().height / 2.0f + bob
                    );
                    star.setFillColor(sf::Color(255, 200, 0));
                    window.draw(star);
                    continue;  // skip the window.draw at the bottom
                }
                    break;
                default:
                {
                    if ((c + r) % 2 == 0)
                    {
                        tile.setFillColor(sf::Color(28, 28, 42));
                    }
                    else
                    {
                        tile.setFillColor(sf::Color(32, 32, 48));
                    }
                    tile.setOutlineThickness(0.0f);
                   
                }
                break;
            }
            window.draw(tile);
        }
    }
}
