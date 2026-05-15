#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include "Form.hpp"

class Level;

// manages input, movement rules, form switching, and the players visual shape
class Player {
public:
    Player();

    void reset(); //return to start cell, Blue form, zero cooldowns
    void handleInput(Level& level, float dt); //read keyboard and advance state by dt seconds
    void draw(sf::RenderWindow& window, float time); //render the shape with pulsing outline

    //grid position accessors
    int  getCol() const { return col; }
    int  getRow() const { return row; }
    Form getForm() const { return form; }

    // audio event flags 
    bool justSwitched = false;
    bool justPushed = false;
    bool justCollectedStar = false;

private:
    void tryMove(int dc, int dr, Level& level); //attempt a one-cell move, enforces all rules
    void toggleForm(); //flip between Blue and Red

    //grid state
    int col = 1; //current grid column
    int row = 1; //current grid row
    Form form = Form::Blue; //active form

    //visual state
    sf::Vector2f visualPos;
    sf::Vector2f targetPos;
    float slideSpeed = 14.f;

    float moveCooldown   = 0.f; //seconds until the next move is allowed
    float switchCooldown = 0.f; //seconds until the next form switch is allowed
    static constexpr float MOVE_DELAY   = 0.15f; //6–7 moves per second when key is held :D
    static constexpr float SWITCH_DELAY = 0.3f; //prevent rapid accidental double-switches

    sf::RectangleShape shape; //the rectangle drawn for the player
};
