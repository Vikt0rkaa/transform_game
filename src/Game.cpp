#include "Game.hpp"
#include <string>


//consturctor
Game::Game()
    : window(sf::VideoMode(WIN_W, WIN_H), "Transform", sf::Style::Close)
{
    window.setFramerateLimit(60); //cap rendering at 60fps so the game runs smoothly and consistently

    if (!font.loadFromFile("assets/Roboto-Regular.ttf")) //try loading the font in order of preference, silent fallback if the first file is missing
        if (!font.loadFromFile("assets/arial.ttf"))
            font.loadFromFile("assets/calibri.ttf");

    loadAudio(); //load and configure all sound effects and bg music
}


//audio setup
void Game::loadAudio() {
    //each sound has a soundbuffer (raw pcm data) and a sound (playback handle), loadfromfile returns false on failure, we only attach the buffer if loading succeeds
    if (switchBuf.loadFromFile("assets/switch.wav"))
        switchSound.setBuffer(switchBuf);
    if (pushBuf.loadFromFile("assets/push.wav"))
        pushSound.setBuffer(pushBuf);
    if (winBuf.loadFromFile("assets/win.wav"))
        winSound.setBuffer(winBuf);
    if (starBuf.loadFromFile("assets/star.wav"))
        starSound.setBuffer(starBuf);

    //different volumes 
    switchSound.setVolume(75.0f);
    pushSound.setVolume(65.0f);
    winSound.setVolume(90.0f);
    starSound.setVolume(85.0f);

    //bg music streams from disk instead of loading everything into ram + its looped and quiet so it doesnt interfere with the sound effects
    if (music.openFromFile("assets/music.wav")) {
        music.setLoop(true);
        music.setVolume(25.0f);
        music.play();
    }
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds(); //dt=seconds elapsed since the last frame
        if (dt > 0.05f) dt = 0.05f; //clamp dt to 50ms, without this a sudden freeze would give a huge dt on the next frame which could result in weird movement
        time += dt; 
        processEvents(); 
        update(dt); 
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) 
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::KeyPressed) 
        {
            if (event.key.code == sf::Keyboard::Escape)
                window.close();
            if (event.key.code == sf::Keyboard::R)
                reset();
            // advance level on enter when level is won, we ignore it otherwise
            if (event.key.code == sf::Keyboard::Return && state == GameState::LevelWon)
                advanceLevel();
        }
    }
}

void Game::update(float dt) {
    if (state != GameState::Playing) return; //player cannot move during "level/game won" screen, we skip all logic

    player.handleInput(level, dt);

    if (player.justSwitched) 
        switchSound.play(); //plays the sfx once
    if (player.justPushed)   
        pushSound.play();
    if (player.justCollectedStar)
    {
        starSound.play();
        starsCollected++; //increment the persistent star counter in hud
    }

    if (level.isGoal(player.getCol(), player.getRow())) //checks whether the player has reached the goal tile
    {
        winSound.play(); //won sound
        music.pause(); //pauses the music
        state = level.isFinalLevel() ? GameState::GameWon : GameState::LevelWon; //transition to the appropriate overlay state
    }
}

//level advancement
void Game::advanceLevel() {
    level.nextLevel();
    player.reset();
    state = GameState::Playing;
    music.play();
}

void Game::render() {
    //clear the screen to a dark blueish grey
    window.clear(sf::Color(18, 18, 30));
    level.draw(window, time, font); //draws the tile grid
    player.draw(window, time); //draws the player
    drawHUD(); //draws the ui overlay
    window.display();
}

void Game::drawHUD() {
    // top bar, semi-transparent black rectangle across the full width, 28px tall
    sf::RectangleShape bar({ (float)WIN_W, 28.0f });
    bar.setFillColor(sf::Color(0, 0, 0, 180)); //alpha 180 = 70% non-transparent
    window.draw(bar);

    // form indicator
    bool isBlue = player.getForm() == Form::Blue;
    std::string formStr = isBlue
        ? "[ BLUE ]  walks on glass"
        : "[ RED  ]  pushes blocks";
    sf::Text formTxt(formStr, font, 14);
    formTxt.setPosition(8.0f, 6.0f);
    formTxt.setFillColor(isBlue ? sf::Color(100, 180, 255) : sf::Color(255, 110, 110));
    window.draw(formTxt);

    // controls hint, centered in hud
    sf::Text hint(" | Tab: switch form | R: restart |", font, 13);
    hint.setFillColor(sf::Color(140, 140, 160));
    hint.setPosition(WIN_W / 2.0f - hint.getLocalBounds().width / 2.0f, 7.0f); //centres the text
    window.draw(hint);

    // star counter 
    float starX = WIN_W - 130.0f;
    float starY = 5.0f;
    for (int i = 0; i < 3; ++i) {
        sf::Text star(i < starsCollected ? "*" : "o", font, 20); //filled star is "*" if collected, hollow "o"
        star.setPosition(starX + i * 36.0f, starY);

        if (i < starsCollected)
        {
            // pulsing gold, oscillates the green channel 
            float pulse = 180.0f + 75.0f * std::sin(time * 3.0f + i);
            star.setFillColor(sf::Color(255, (sf::Uint8)pulse, 0));
        }
        else 
        {
            // dim grey for uncollected stars
            star.setFillColor(sf::Color(80, 80, 100));
        }
        window.draw(star);
    }

    // level indicator
    std::string lvlStr = "Level " + std::to_string(level.getCurrentLevel() + 1) + "/3";
    sf::Text lvlTxt(lvlStr, font, 13);
    lvlTxt.setFillColor(sf::Color(160, 160, 180));
    lvlTxt.setPosition(starX - lvlTxt.getLocalBounds().width - 12.0f, 7.0f);
    window.draw(lvlTxt);

    // level won overlay
    if (state == GameState::LevelWon) {
        sf::RectangleShape overlay({ (float)WIN_W, (float)WIN_H }); //darken the whole screen
        overlay.setFillColor(sf::Color(0, 0, 0, 160));
        window.draw(overlay);

        sf::Text t("Level Complete!", font, 40);
        t.setFillColor(sf::Color(255, 215, 0));
        t.setPosition(WIN_W / 2.0f - t.getLocalBounds().width / 2.0f, WIN_H / 2.0f - 60.0f);
        window.draw(t);

        // show if star was collected
        std::string starMsg = level.isStarCollected()
            ? "*  Star collected!"
            : "o  You missed the star :(";
        sf::Text starTxt(starMsg, font, 18);
        starTxt.setFillColor(level.isStarCollected()
            ? sf::Color(255, 215, 0)
            : sf::Color(160, 160, 180));
        starTxt.setPosition(WIN_W / 2.0f - starTxt.getLocalBounds().width / 2.0f, WIN_H / 2.0f);
        window.draw(starTxt);

        sf::Text sub("Enter: next level   R: restart level", font, 16);
        sub.setFillColor(sf::Color(180, 180, 200));
        sub.setPosition(WIN_W / 2.0f - sub.getLocalBounds().width / 2.0f, WIN_H / 2.0f + 40.0f);
        window.draw(sub);
    }

    // game won overlay
    if (state == GameState::GameWon) {
        sf::RectangleShape overlay({ (float)WIN_W, (float)WIN_H }); //slightly less transparent than level won to make it feel more final
        overlay.setFillColor(sf::Color(0, 0, 0, 170));
        window.draw(overlay);

        sf::Text t("You Escaped!", font, 48);
        t.setFillColor(sf::Color(255, 215, 0));
        t.setPosition(WIN_W / 2.0f - t.getLocalBounds().width / 2.0f, WIN_H / 2.0f - 80.0f);
        window.draw(t);

        // big star display of total stars collected
        std::string bigStars = "";
        for (int i = 0; i < 3; ++i)
            bigStars += (i < starsCollected ? "* " : "o");
        sf::Text starDisplay(bigStars, font, 48);
        starDisplay.setFillColor(sf::Color(255, 200, 0));
        starDisplay.setPosition(WIN_W / 2.0f - starDisplay.getLocalBounds().width / 2.0f, WIN_H / 2.0f - 10.0f);
        window.draw(starDisplay);

        sf::Text score(std::to_string(starsCollected) + " / 3 stars", font, 22);
        score.setFillColor(sf::Color(200, 200, 220));
        score.setPosition(WIN_W / 2.0f - score.getLocalBounds().width / 2.0f, WIN_H / 2.0f + 50.0f);
        window.draw(score);

        sf::Text sub("R to play again | Escape to quit", font, 16);
        sub.setFillColor(sf::Color(140, 140, 160));
        sub.setPosition(WIN_W / 2.0f - sub.getLocalBounds().width / 2.0f, WIN_H / 2.0f + 86.0f);
        window.draw(sub);
    }
}

//reset
void Game::reset() {
    level.reset();
    player.reset();
    if (level.isStarCollected() && starsCollected > 0) //if player collected the star, it subtracts it
        starsCollected--;
    state = GameState::Playing;
    music.play();
}
