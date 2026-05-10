#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include "Form.hpp"

class Level;

class Player {
public:
    Player();

    void reset();
    void handleInput(Level& level, float dt);
    void draw(sf::RenderWindow& window, float time);

    int  getCol() const { return col; }
    int  getRow() const { return row; }
    Form getForm() const { return form; }

    // audio event flags 
    bool justSwitched = false;
    bool justPushed = false;
    bool justCollectedStar = false;

private:
    void tryMove(int dc, int dr, Level& level);
    void toggleForm();

    int col = 1;
    int row = 1;
    Form form = Form::Blue;

    sf::Vector2f visualPos;
    sf::Vector2f targetPos;
    float slideSpeed = 14.f;

    float moveCooldown   = 0.f;
    float switchCooldown = 0.f;
    static constexpr float MOVE_DELAY   = 0.15f;
    static constexpr float SWITCH_DELAY = 0.3f;

    sf::RectangleShape shape;
};
