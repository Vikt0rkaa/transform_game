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
    "#....#....#.B..G..#",
    "#.#.#####.#.#####.#",
    "#.....S.#...G.....#",
    "###################"
};

//LEVEL 3
static const char* MAP3[ROWS] = {
    "###################",
    "#..#......B#GG...S#",
    "#B##.#####.#G#.####",
    "#.##.#.....B..##X.#",
    "#..#.GG###G######.#",
    "#...B###.#.#.GGG#.#",
    "###G.#.#.#B#.###..#",
    "#..G##..B..#.#...##",
    "#.####.#G#####.#G.#",
    "#.#....BB...#..#GB#",
    "#.#G####..B.#####.#",
    "#.B..#...#.GGGGG..#",
    "###################"
};

//pointer array so we can index maps by level number 0,1,2
static const char** MAPS[3] = {
    (const char**)MAP1,
    (const char**)MAP2,
    (const char**)MAP3
};

//constructor
Level::Level() 
{
    buildMap();
}

//converts the raw character map into typed tile grid, must be called whenever currentLevel changes or after a reset
void Level::buildMap() {
    const char** map = MAPS[currentLevel];
    for (int r = 0; r < ROWS; ++r) 
    {
        for (int c = 0; c < COLS; ++c) 
        {
            char ch = map[r][c]; //map each character to its corresponding tile enum value
            if (ch == '#') grid[r][c] = Tile::Wall;
            else if (ch == 'G') grid[r][c] = Tile::Glass;
            else if (ch == 'B') grid[r][c] = Tile::Block;
            else if (ch == 'X') grid[r][c] = Tile::Goal;
            else if (ch == 'S')grid[r][c] = Tile::Star;
            else grid[r][c] = Tile::Empty; //'.' or anything else
        }
        starCollected = false; //reset the star-collected flag every time the map is built
    }
}

//rebuilds the current level including the star
void Level::reset() 
{
    buildMap();
}

//advances to new level, max index 2
void Level::nextLevel() 
{
    if (currentLevel < 2)
        ++currentLevel; //only increment if were not already on the last level
    buildMap();
}

//returns true if player is on the 3rd level index 2
bool Level::isFinalLevel() const 
{
    return currentLevel == 2;
}

//returns the index 0,1,2
int Level::getCurrentLevel() const 
{
    return currentLevel;
}

//returns whether the star is collected
bool Level::isStarCollected() const
{
    return starCollected;
}

//returns tile position, any out-of-bounds position is treated as a wall 
Tile Level::getTile(int c, int r) const 
{
    if (c < 0 || c >= COLS || r < 0 || r >= ROWS)
        return Tile::Wall;
    return grid[r][c];
}

//writes a new tile value, used by player to slide blocks into their destination cell
void Level::setTile(int c, int r, Tile t) 
{
    if (c < 0 || c >= COLS || r < 0 || r >= ROWS)
        return;
    grid[r][c] = t;
}

bool Level::isSolid(int c, int r) const //true if the tile blocks movement
{ auto t = getTile(c,r); return t == Tile::Wall || t == Tile::Block; }
bool Level::isGlass(int c, int r) const //true only for glass tiles, relevant for form rules
{ return getTile(c,r) == Tile::Glass; }
bool Level::isBlock(int c, int r) const //true only if pushable
{ return getTile(c,r) == Tile::Block; }
bool Level::isGoal(int c, int r) const //true only for the goal, reaching it wins the level
{ return getTile(c,r) == Tile::Goal;  }

bool Level::tryCollectStar(int c, int r) //called when the player moves onto a tile, if that tile  is star it is replaced with empty, returns true if a star was collected
{
    if (getTile(c, r) == Tile::Star)
    {
        setTile(c, r, Tile::Empty); //remove star
        starCollected = true; //remember it was collected
        return true;
    }
    return false; //no star at this position
}

//render every cell of the tile grid to the window, 'time' drives animations (pulsing goal, bobbing star), 'font' is needed for the rendering of the '*'
void Level::draw(sf::RenderWindow& window, float time, sf::Font& font)
{
    sf::RectangleShape tile(sf::Vector2f(TILE - 1.0f, TILE - 1.0f)); //reuse a single rectangleshape for every tile, only position and color changes, 1px gap for grid lines

    for (int r = 0; r < ROWS; ++r) 
    {
        for (int c = 0; c < COLS; ++c) 
        {
            tile.setPosition(c * TILE + 0.5f, r * TILE + 0.5f); //position the tile shape 0,5px offset centers the 1px gap between tiles 
            tile.setOutlineThickness(0); //reset outline for each tile, some tiles set it

            switch (grid[r][c]) 
            {
                case Tile::Wall: //dark blue-grey with lighter outline
                {
                    tile.setFillColor(sf::Color(55, 55, 78));
                    tile.setOutlineColor(sf::Color(90, 90, 120));
                    tile.setOutlineThickness(1.0f);
                }
                break;
                case Tile::Glass: //semi-transparent, sky-blue
                {
                    tile.setFillColor(sf::Color(140, 210, 255, 160));
                    tile.setOutlineColor(sf::Color(180, 230, 255, 220));
                    tile.setOutlineThickness(1.5f);
                }
                break;
                case Tile::Block: //solid red
                {
                    tile.setFillColor(sf::Color(180, 40, 40));
                    tile.setOutlineColor(sf::Color(120, 10, 10));
                    tile.setOutlineThickness(1.5f);
                }
                break;
                case Tile::Goal: //pulsing yellow orange
                {
                    float pulse = 180.0f + 75.0f * std::sin(time * 3.0f);
                    tile.setFillColor(sf::Color(255, (sf::Uint8)pulse, 0));
                    tile.setOutlineColor(sf::Color(255, 220, 0));
                    tile.setOutlineThickness(2.0f);
                }
                break;
                case Tile::Star: //the star tile, we draw manually continue to skip the bottom window.draw call
                {
                    tile.setFillColor(sf::Color(60, 0, 100));
                    tile.setOutlineColor(sf::Color(180, 80, 255));
                    tile.setOutlineThickness(2.0f);
                    window.draw(tile); //draws the purple bg 1st

                    sf::Text star("*", font, 28);
                    float bob = 6.0f * std::sin(time * 4.0f); //+- 6px vertical oscillation
                    star.setPosition(
                        c * TILE + TILE / 2.f - star.getLocalBounds().width / 2.0f,
                        r * TILE + TILE / 2.f - star.getLocalBounds().height / 2.0f + bob
                    );
                    star.setFillColor(sf::Color(255, 200, 0));
                    window.draw(star);
                    continue;  // skip the window.draw at the bottom
                }
                    break;
                default: //empty floor, subtle checkerboard pattern
                {
                    if ((c + r) % 2 == 0)
                    {
                        tile.setFillColor(sf::Color(28, 28, 42)); //darker
                    }
                    else
                    {
                        tile.setFillColor(sf::Color(32, 32, 48)); //lighter
                    }
                    tile.setOutlineThickness(0.0f); //no outline
                   
                }
                break;
            }
            window.draw(tile); //draw the tile, not reached for star due to continue
        }
    }
}
