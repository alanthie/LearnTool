#pragma once

#include <functional>
#include "Widget.h"
#include "Button.h"
#include "Quiz.h"

typedef void (StateBase::*StateBaseMemFn)(std::string& name);

namespace gui
{
    class GuiQuiz : public gui::Widget
    {
    public:
        GuiQuiz(const std::string& name, float _w, float _h, float _h_text);
        void reset();
        bool load_quiz(const std::string& filename);
        bool is_loaded(const std::string& filename);

        void setFunction(StateBaseMemFn f) { m_state_func = f; }
        void setTexture(const sf::Texture& texture);

        void handleEvent(sf::Event e, const sf::RenderWindow& window, StateBase& current_state) override;
        void render(sf::RenderTarget& renderer) override;
        void setPosition(const sf::Vector2f& pos)   override;

        sf::Vector2f getSize() const override;

        void load();
        bool isAnswerIndexSelected(size_t index) const;
        bool isAnswerOK() const;

        float w;
        float h;
        float h_text;
        sf::Vector2f    m_position;
        Rectangle       m_rect;
        StateBaseMemFn  m_state_func;

        bool            m_is_loaded = false;
        std::string     m_filename = "";
        Quiz            m_quiz;

        std::shared_ptr<Button>                 b_subject;
        std::shared_ptr<Button>                 b_question;
        std::vector<std::shared_ptr<Button>>    b_choices;
        std::vector<std::shared_ptr<Button>>    b_answers;
        std::shared_ptr<Button>                 b_result;
   
        size_t answer_index_clicked = 0;
    };
}