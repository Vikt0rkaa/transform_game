#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Constants.hpp"
#include "Level.hpp"
#include "Player.hpp"

//tracks which overlay is active
enum class GameState { Playing, LevelWon, GameWon };

//the root object that owns window, audio, level, and player constructed in main
class Game {
public:
    Game(); //constructor: opens the window, loads assets, initialises state
    void run(); //blocking main loop, returns only when the window is closed

private:
    void processEvents(); //drain  the os event queue
    void update(float dt); //advance game logic
    void render(); //clear, draw level, player, hud, then display
    void drawHUD(); //top bar, form label, star counter, win overlays
    void reset(); //restart the current level 
    void loadAudio(); //load WAV files and configure volumes
    void advanceLevel(); //move to the next level and resume play

    //sfml objects
    sf::RenderWindow window; //the os window and a 2d renderer
    sf::Font font;
    GameState state = GameState::Playing;
    float time = 0.0f; //total accumulated time used for animations

    Level level; //owns the tile grid and level data
    Player player; //owns movement, form, shape

    int starsCollected = 0; //persistent count across all levels (max 3)

    // audio
    sf::SoundBuffer switchBuf, pushBuf, winBuf, starBuf;
    sf::Sound switchSound, pushSound, winSound, starSound;
    sf::Music music;
};
