#pragma once

#include <functional>

#include "Widget.h"

namespace gui
{
    enum class ButtonSize
    {
        Small,
        Wide,
    };

    class Button : public gui::Widget
    {
        public:
            Button(const std::string& n, ButtonSize s = ButtonSize::Wide);

            void setFunction(std::function<void(StateBase& g, std::string& n)> func);
            void setText    (const std::string& str);
            void setTexture (const sf::Texture& tex);

            void handleEvent    (sf::Event e, const sf::RenderWindow& window, StateBase& g) override;
            void render         (sf::RenderTarget& renderer) override;
            void setPosition    (const sf::Vector2f& pos)   override;
            sf::Vector2f getSize() const    override;

        //protected:
            void updateText();

            sf::Vector2f    m_position;

            Rectangle   m_button;
            Text        m_text;
            std::function<void(StateBase& g, std::string& n)> m_function;// = []() {};

    };

    inline std::unique_ptr<Button> makeButton(const std::string& n) { return std::make_unique<Button>(n); }
}