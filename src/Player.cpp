#include "Player.hpp"
#include "Level.hpp"
#include <cmath>

Player::Player() {
    shape.setSize({ TILE * 0.5f, TILE * 0.5f });
    shape.setFillColor(sf::Color(60, 130, 255));
    shape.setOutlineColor(sf::Color(140, 200, 255));
    shape.setOutlineThickness(2.f);
    reset();
}

void Player::reset() {
    col = 1;
    row = 1;
    form = Form::Blue;
    visualPos = { col * TILE + TILE * 0.25f, row * TILE + TILE * 0.25f };
    targetPos = visualPos;
    moveCooldown = 0.0f;
    switchCooldown = 0.0f;
    justSwitched = false;
    justPushed = false;
    justCollectedStar = false;
    shape.setSize({ TILE * 0.5f, TILE * 0.5f });
    shape.setFillColor(sf::Color(60, 130, 255));
    shape.setOutlineColor(sf::Color(140, 200, 255));
}

void Player::toggleForm() 
{
    if (form == Form::Blue) 
    {
        form = Form::Red;
        shape.setSize({ TILE * 0.72f, TILE * 0.72f });
        shape.setFillColor(sf::Color(220, 60, 60));
        shape.setOutlineColor(sf::Color(255, 140, 140));
    } else 
    {
        form = Form::Blue;
        shape.setSize({ TILE * 0.5f, TILE * 0.5f });
        shape.setFillColor(sf::Color(60, 130, 255));
        shape.setOutlineColor(sf::Color(140, 200, 255));
    }
    justSwitched = true;
}

void Player::tryMove(int dc, int dr, Level& level) 
{
    int nc = col + dc;
    int nr = row + dr;
    Tile target = level.getTile(nc, nr);

    if (target == Tile::Wall)
    {
        return;
    }

    if (target == Tile::Glass && form == Form::Red)
    {
        return;
    }

    if (target == Tile::Block) 
    {
        if (form == Form::Blue)
        {
            return;
        }

        int bc = nc + dc;
        int br = nr + dr;
        Tile beyond = level.getTile(bc, br);
        if (beyond == Tile::Wall || beyond == Tile::Block || beyond == Tile::Glass)
        {
            return;
        }

        level.setTile(nc, nr, Tile::Empty);
        level.setTile(bc, br, Tile::Block);
        justPushed = true;
    }

    col = nc;
    row = nr;
    targetPos = { col * TILE + (TILE - shape.getSize().x) / 2.0f,
                  row * TILE + (TILE - shape.getSize().y) / 2.0f };
    if (level.tryCollectStar(col, row))
    {
        justCollectedStar = true;
    }
}

void Player::handleInput(Level& level, float dt) 
{
    moveCooldown -= dt;
    switchCooldown -= dt;

    justSwitched = false;
    justPushed = false;
    justCollectedStar = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab) && switchCooldown <= 0.0f) 
    {
        toggleForm();
        switchCooldown = SWITCH_DELAY;
        targetPos = { col * TILE + (TILE - shape.getSize().x) / 2.0f,
                      row * TILE + (TILE - shape.getSize().y) / 2.0f };
    }

    if (moveCooldown <= 0.0f) 
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            tryMove(0, -1, level);
            moveCooldown = MOVE_DELAY;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            tryMove(0, 1, level);
            moveCooldown = MOVE_DELAY;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            tryMove(-1, 0, level);
            moveCooldown = MOVE_DELAY;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            tryMove(1, 0, level);
            moveCooldown = MOVE_DELAY;
        }
    }

    visualPos += (targetPos - visualPos) * slideSpeed * dt;
}

void Player::draw(sf::RenderWindow& window, float time) 
{
    float pulse = 0.7f + 0.3f * std::sin(time * 5.0f);
    sf::Color outline = shape.getOutlineColor();
    outline.a = (sf::Uint8)(200 * pulse);
    shape.setOutlineColor(outline);
    shape.setPosition(visualPos);
    window.draw(shape);
}
