#include "Game.hpp"

int main() 
{
    Game game; //create the game object, sets up the window, loads assets, initializes state
    game.run(); // enter the main loop, blocks until the window is closed
    return 0; 
}
