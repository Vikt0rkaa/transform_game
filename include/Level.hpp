#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include "Constants.hpp"

class Level {
public:
    Level();

    void reset();       // restart current level
    void nextLevel();   // advance to next level
    void buildMap();

    bool isFinalLevel() const;
    int  getCurrentLevel() const;
    bool isStarCollected() const;

    bool tryCollectStar(int col, int row); //returns true if a star is collected 

    void draw(sf::RenderWindow& window, float time, sf::Font& font);

    Tile getTile(int col, int row) const;
    void setTile(int col, int row, Tile t);

    bool isSolid(int col, int row) const;
    bool isGlass(int col, int row) const;
    bool isBlock(int col, int row) const;
    bool isGoal(int col, int row)  const;

private:
    std::array<std::array<Tile, COLS>, ROWS> grid;
    int currentLevel = 0;   // 0, 1, 2
    bool starCollected = false;
};
