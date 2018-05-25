#include "Button.h"
#include <iostream>

namespace gui {

Button::Button(const std::string& name, ButtonSize s) : gui::Widget(name)
{
    m_rect.setOutlineThickness(1);
    m_rect.setOutlineColor(sf::Color::Green);
    m_rect.setFillColor(sf::Color::Black);
    switch (s) 
    {
        case ButtonSize::Wide:
            m_rect.setSize({256, 64});
            break;

        case ButtonSize::Small:
            m_rect.setSize({128, 64});
            break;
    }
}

void Button::setText (const std::string& str)
{
    m_text.setString(str);
    updateText();
}

void Button::setTexture (const sf::Texture& tex)
{
    m_rect.setTexture(&tex);
}

void Button::handleEvent(sf::Event e, const sf::RenderWindow& window, StateBase& current_state)
{
    auto pos = sf::Mouse::getPosition(window);

    switch(e.type) 
    {
        case sf::Event::MouseButtonPressed:
            switch(e.mouseButton.button) 
            {
                case sf::Mouse::Left:
                    if (m_rect.getGlobalBounds().contains((float)pos.x, (float)pos.y)) 
                    {
                        std::invoke(m_state_func, &current_state, name); // current_state->m_state_func(name)
                    }

                default:
                    break;
            }

        default:
            break;
    }
}

void Button::render(sf::RenderTarget& renderer)
{
    renderer.draw(m_rect);
    renderer.draw(m_text);
}

void Button::setPosition(const sf::Vector2f& pos)
{
    m_position = pos;

    m_rect.setPosition(m_position);
    m_text.setPosition  (m_position);

    updateText();
}

void Button::updateText()
{
    m_text.setOrigin(   m_text.getGlobalBounds().width  / 2,        m_text.getGlobalBounds().height / 2);
    m_text.move(        m_rect.getGlobalBounds().width  / 2.0f,   m_rect.getGlobalBounds().height / 2.5f);
}

sf::Vector2f Button::getSize() const
{
    return m_rect.getSize();
}

}
