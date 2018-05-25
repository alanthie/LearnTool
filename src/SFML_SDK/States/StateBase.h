#pragma once

#include <SFML/Graphics.hpp>
#include "../Util/NonCopyable.h"

class Game;

class StateBase
{
    public:
        StateBase(Game& game) : m_pGame(&game) {}

        virtual void b_click(std::string& b_name) {}

        virtual ~StateBase() = default;

        virtual void handleEvent(sf::Event e) {}
        virtual void handleInput() = 0;
        virtual void update(sf::Time deltaTime) {}
        virtual void fixedUpdate(sf::Time deltaTime) {}
        virtual void render(sf::RenderTarget& renderer) = 0;

        Game* m_pGame;
};
