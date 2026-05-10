#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Constants.hpp"
#include "Level.hpp"
#include "Player.hpp"

enum class GameState { Playing, LevelWon, GameWon };

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void drawHUD();
    void reset();
    void loadAudio();
    void advanceLevel();

    sf::RenderWindow window;
    sf::Font font;
    GameState state = GameState::Playing;
    float time = 0.0f;

    Level level;
    Player player;

    int starsCollected = 0;

    // audio
    sf::SoundBuffer switchBuf, pushBuf, winBuf, starBuf;
    sf::Sound switchSound, pushSound, winSound, starSound;
    sf::Music music;
};
