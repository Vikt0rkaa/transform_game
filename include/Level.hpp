#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include "Constants.hpp"

//level manages the tile grid for the current level
class Level {
public:
    Level();

    void reset();       // restart current level
    void nextLevel();   // advance to next level
    void buildMap();

    bool isFinalLevel() const; //true when currentlevel is 2
    int  getCurrentLevel() const; //returns 0,1,2
    bool isStarCollected() const; //true after the star tile has been walked on

    //attempts to collect the star, returns true is star was there
    bool tryCollectStar(int col, int row);  

    //draws all tiles to the window, `time` drives animations, `font` renders the star
    void draw(sf::RenderWindow& window, float time, sf::Font& font);

    Tile getTile(int col, int row) const;
    void setTile(int col, int row, Tile t);

    bool isSolid(int col, int row) const;
    bool isGlass(int col, int row) const;
    bool isBlock(int col, int row) const;
    bool isGoal(int col, int row)  const;

private:
    std::array<std::array<Tile, COLS>, ROWS> grid; //2d array 
    int currentLevel = 0;   // 0, 1, 2
    bool starCollected = false;
};
