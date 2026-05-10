#pragma once

// grid 
constexpr int COLS = 19;
constexpr int ROWS = 13;
constexpr int TILE = 48;  // pixels per tile

// window size
constexpr int WIN_W = COLS * TILE;  
constexpr int WIN_H = ROWS * TILE; 

// tile types 
enum class Tile {
    Empty,    // walkable floor
    Wall,     // solid wall
    Glass,    // only Blue can walk on it
    Block,    // pushable only by Red
    Goal,    // reach this to win
    Star,   //collectible, one per level
};
