#include "Player.hpp"
#include "Level.hpp"
#include <cmath>

Player::Player() {
    //set the shape size for the blue form, small square 50% of a tile
    shape.setSize({ TILE * 0.5f, TILE * 0.5f });
    shape.setFillColor(sf::Color(60, 130, 255));
    shape.setOutlineColor(sf::Color(140, 200, 255));
    shape.setOutlineThickness(2.f);
    reset(); //initialise position, form, and cooldown variables
}

//reset, returns the player to the start position, state to blue
void Player::reset() {
    col = 1; //starting column
    row = 1; //starting row
    form = Form::Blue;
    visualPos = { col * TILE + TILE * 0.25f, row * TILE + TILE * 0.25f }; //position that centres the shape
    targetPos = visualPos; //no animation
    moveCooldown = 0.0f; //allow movement immediately
    switchCooldown = 0.0f; //allow form switch immediately
    justSwitched = false;
    justPushed = false;
    justCollectedStar = false;
    shape.setSize({ TILE * 0.5f, TILE * 0.5f }); //Restore blue form visual properties, in case reset is called after switching to red
    shape.setFillColor(sf::Color(60, 130, 255));
    shape.setOutlineColor(sf::Color(140, 200, 255));
}

//switches the player between blue and red
void Player::toggleForm() 
{
    if (form == Form::Blue) 
    {
        form = Form::Red; //red form 72% of the tile
        shape.setSize({ TILE * 0.72f, TILE * 0.72f });
        shape.setFillColor(sf::Color(220, 60, 60));
        shape.setOutlineColor(sf::Color(255, 140, 140));
    } else 
    {
        form = Form::Blue; //blue 50% of the tile
        shape.setSize({ TILE * 0.5f, TILE * 0.5f });
        shape.setFillColor(sf::Color(60, 130, 255));
        shape.setOutlineColor(sf::Color(140, 200, 255));
    }
    justSwitched = true; 
}

//attenpts to move the player by (dc, dr) grif cells, enforces all rules, if move is valid the position is updated
void Player::tryMove(int dc, int dr, Level& level) 
{
    int nc = col + dc;
    int nr = row + dr;
    Tile target = level.getTile(nc, nr); //tile type at the destination

    if (target == Tile::Wall)
    {
        return; //wall always blocks movement
    }

    if (target == Tile::Glass && form == Form::Red)
    {
        return; //red cannot step on the glass tile
    }

    if (target == Tile::Block) 
    {
        if (form == Form::Blue)
        {
            return; //block is only pushable by red and not blue
        }

        int bc = nc + dc; //column the block would slide onto
        int br = nr + dr; //same for row
        Tile beyond = level.getTile(bc, br); //what is beyond the block
        if (beyond == Tile::Wall || beyond == Tile::Block || beyond == Tile::Glass) //block cannot be pushed if the cell behind is a wall, another block or glass
        {
            return;
        }

        level.setTile(nc, nr, Tile::Empty); //perform the push
        level.setTile(bc, br, Tile::Block);
        justPushed = true;
    }

    col = nc;
    row = nr;
    targetPos = { col * TILE + (TILE - shape.getSize().x) / 2.0f, //recalculate the pixel target, centring the shape within the new tile
                  row * TILE + (TILE - shape.getSize().y) / 2.0f };
    if (level.tryCollectStar(col, row)) //check if the destination tile contained a star, collect if so
    {
        justCollectedStar = true;
    }
}

//called every frame during gameplay, reads keyboard state, applies cooldowns, and updates visual position
void Player::handleInput(Level& level, float dt) 
{
    moveCooldown -= dt; //tick down both cooldown timers by the frames delta times
    switchCooldown -= dt;

    justSwitched = false; //reset single-frame event flags
    justPushed = false;
    justCollectedStar = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab) && switchCooldown <= 0.0f) //delay prevents the form from flickering when tab is held down
    {
        toggleForm();
        switchCooldown = SWITCH_DELAY;
        targetPos = { col * TILE + (TILE - shape.getSize().x) / 2.0f, //recalculates the target pixel position because the shape size changed
                      row * TILE + (TILE - shape.getSize().y) / 2.0f };
    }

    if (moveCooldown <= 0.0f)  //rate-limits movement so the player advances one tile per input, each branch passes the corresponding delta (dc, dr) to tryMove, only one direction is processed per frame
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

    //smooth sliding animation
    visualPos += (targetPos - visualPos) * slideSpeed * dt;
}

//renders the player shape and current visual position
void Player::draw(sf::RenderWindow& window, float time) 
{
    float pulse = 0.7f + 0.3f * std::sin(time * 5.0f); //pulse 
    sf::Color outline = shape.getOutlineColor(); //only on the outline the fill color stays the same
    outline.a = (sf::Uint8)(200 * pulse);
    shape.setOutlineColor(outline);
    shape.setPosition(visualPos);
    window.draw(shape);
}
