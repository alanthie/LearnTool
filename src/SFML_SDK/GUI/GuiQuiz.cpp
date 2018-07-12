#include "GuiQuiz.h"
#include <iostream>

namespace gui {

    GuiQuiz::GuiQuiz(const std::string& name, float _w, float _h, float _h_text) : gui::Widget(name), w(_w), h(_h), h_text(_h_text)
    {
        m_rect.setOutlineThickness(1);
        m_rect.setOutlineColor(sf::Color::Green);
        m_rect.setFillColor(sf::Color::White);
        m_rect.setSize({ _w, _h });
    }

    bool GuiQuiz::is_loaded(const std::string& filename)
    {
        if (m_is_loaded == true)
        {
            if (m_filename == filename)
            {
                return true;
            }
        }
        return false;
    }

    bool GuiQuiz::load_quiz(const std::string& filename)
    {
        if (is_loaded(filename) == true)
            return true;

        reset();
        int r = m_quiz.read_xml(filename);

        if (r == XML_SUCCESS)
        {
            m_is_loaded = true;
            m_filename = filename;
            load();
            return true;
        }
        else
        {
            m_is_loaded = false;
            return false;
        }
    }

    void GuiQuiz::reset()
    {
        m_is_loaded = false;
        m_filename.clear();
        m_quiz.clear();

        b_subject.reset();
        b_question.reset();
        b_choices.clear();
        b_answers.clear();
        b_result.reset();
    }

    void GuiQuiz::load()
    {
        m_rect.setSize(sf::Vector2f(w, h_text * (3 + m_quiz._choice.size()) ) );

        b_subject = makeSharedButton("subject");
        b_subject->m_rect.setSize({ w, h_text });
        b_subject->setText(m_quiz._subject);
        b_subject->setPosition(m_rect.getPosition() + sf::Vector2f(0.0f, 0 * h_text));

        b_question = makeSharedButton("question");
        b_question->m_rect.setSize({ w, h_text });
        b_question->setText(m_quiz._question);
        b_question->setPosition(m_rect.getPosition() + sf::Vector2f(0.0f, 1 * h_text) );

        for (size_t i = 0; i < m_quiz._choice.size(); i++)
        {
            std::shared_ptr<Button> b = makeSharedButton("choice_" + std::to_string(i));
            b->m_rect.setSize({ w - 50 * 3, h_text });
            b->setText(std::to_string(i+1) + ". " + m_quiz._choice[i]._text);
            b->setPosition(m_rect.getPosition() + sf::Vector2f(0.0f, (i+2) * h_text));

            b_choices.push_back(b);

            std::shared_ptr<Button> ba = makeSharedButton("answer_" + std::to_string(i));
            ba->m_rect.setSize({ 50 * 3, h_text });
            ba->setText("[ ]");
            ba->setPosition(sf::Vector2f(w - 50 * 3, 0.0f) + sf::Vector2f(0.0f, (i + 2) * h_text));
            b_answers.push_back(ba);
        }

        b_result = makeSharedButton("result");
        b_result->m_rect.setSize({ w, h_text });
        b_result->setText(" ");
        b_result->setPosition(m_rect.getPosition() + sf::Vector2f(0.0f, (m_quiz._choice.size() + 2) * h_text));
    }

    void GuiQuiz::setTexture(const sf::Texture& tex)
    {
        m_rect.setTexture(&tex);
    }

    void GuiQuiz::handleEvent(sf::Event e, const sf::RenderWindow& win, StateBase& current_state)
    {
        auto pos = sf::Mouse::getPosition(win);
        sf::RenderWindow& window = (sf::RenderWindow&) (win);

        if (m_is_loaded == false)
            return;

        switch (e.type)
        {
        case sf::Event::MouseButtonPressed:
            switch (e.mouseButton.button)
            {
            case sf::Mouse::Left:
                //if (m_rect.getGlobalBounds().contains((float)pos.x, (float)pos.y))
                {
                    for (size_t i = 0; i < b_choices.size(); i++)
                    {
                        if (b_answers[i]->m_rect.getGlobalBounds().contains((float)pos.x, (float)pos.y))
                        {
                            answer_index_clicked = i;
                            std::string s = b_answers[i]->m_text.getString();
                            if (s == "[ ]") s = "[X]";
                            else s = "[ ]";
                            //b_answers[i]->m_text.setString(s);
                            b_answers[i]->setText(s);

                            if (isAnswerOK() == true)
                            {
                                b_result->setText(" Bravo! ");
                            }
                            else
                            {
                                b_result->setText(" ... ");
                            }

                            std::invoke(m_state_func, &current_state, name);
                            break;
                        }
                    }
                }

            default:
                break;
            }

        default:
            break;
        }
    }

    bool GuiQuiz::isAnswerIndexSelected(size_t index) const
    {
        if ((index < 0) || (index >= b_answers.size()))
            return false;

        std::string s = b_answers[index]->m_text.getString();
        if (s == "[X]") return true;
        return false;
    }

    bool GuiQuiz::isAnswerOK() const
    {
        if (b_answers.size() == 0) return true;
        for (size_t i = 0; i < m_quiz._choice.size(); i++)
        {
            bool selected = isAnswerIndexSelected(i);
            if ((m_quiz._choice[i]._is_true == true) && (selected == false)) return false;
            if ((m_quiz._choice[i]._is_true == false) && (selected == true)) return false;
        }
        return true;
    }


    void GuiQuiz::render(sf::RenderTarget& renderer)
    {
        if (m_is_loaded)
        {
            if (isAnswerOK() == true)
            {
                b_result->setText(" Bravo! ");
            }
            else
            {
                b_result->setText(" ... ");
            }

            renderer.draw(m_rect);
            b_subject->render(renderer);
            b_question->render(renderer);
            for (size_t i = 0; i < b_choices.size(); i++)
            {
                b_choices[i]->render(renderer);
                b_answers[i]->render(renderer);
            }
            b_result->render(renderer);
        }
    }

    void GuiQuiz::setPosition(const sf::Vector2f& pos)
    {
        m_position = pos;
        m_rect.setPosition(m_position);

        if (m_is_loaded)
        {
            b_subject->setPosition(m_rect.getPosition() + sf::Vector2f(0.0f, 0 * h_text));
            b_question->setPosition(m_rect.getPosition() + sf::Vector2f(0.0f, 1 * h_text));

            for (size_t i = 0; i < m_quiz._choice.size(); i++)
            {
                b_choices[i]->setPosition(m_rect.getPosition() + sf::Vector2f(0.0f, (i + 2) * h_text));
                b_answers[i]->setPosition(sf::Vector2f(m_rect.getPosition().x + m_rect.getSize().x - 50 * 3, 0.0f) + sf::Vector2f(0.0f, (i + 2) * h_text));
            }
            b_result->setPosition(m_rect.getPosition() + sf::Vector2f(0.0f, (m_quiz._choice.size() + 2) * h_text));
        }
    }

    sf::Vector2f GuiQuiz::getSize() const
    {
        return m_rect.getSize();
    }
}
