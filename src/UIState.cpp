//
//
#include "UIState.h"
#include "SFML_SDK/Game.h"
#include "SFML_SDK/GUI/Button.h"
#include "SFML_SDK/GUI/Textbox.h"
#include "SFML_SDK/States/StateBase.h"
#include "SFML_SDK/GUI/StackMenu.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

void UIState::b_click(std::string& b_name)
{
    std::cout << "button" << b_name << " clicked!" << '\n';
    if (b_name == "b_pause")
    {
        is_pause = !is_pause;
		if (is_pause)
			button_pause.setText("continue");
		else
			button_pause.setText("pause");
    }
}

std::vector<std::string> split(const std::string &text, char sep) 
{
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

std::string merge(std::vector<std::string> v)
{
    std::string s;
    for (size_t i=0;i<v.size();i++)
    {
        s += v[i]; 
        if (i < v.size() - 1)
            s += ", ";
    }
    return s;
}


UIState::UIState(UImain& g) : StateBase(g),
    ui(g),
    button_pause("b_pause", gui::ButtonSize::Small),
    button_name("b_name",   gui::ButtonSize::Small),
    button_parts("b_parts", gui::ButtonSize::Wide),
	button_prev("b_prev"  , gui::ButtonSize::Wide)
{
    button_pause.setFunction(   &StateBase::b_click);
    button_name.setFunction(    &StateBase::b_click);
    button_parts.setFunction(   &StateBase::b_click);
	button_prev.setFunction(	&StateBase::b_click);
	button_pause.setText("pause");
    load_path(filesystem::path("..\\chufa (leave_stem, oil, root)"));
}

void UIState::handleEvent(sf::Event e) 
{
    button_pause.handleEvent(e, m_pGame->getWindow(), *this);
    button_name.handleEvent(e, m_pGame->getWindow(), *this);
    button_parts.handleEvent(e, m_pGame->getWindow(), *this);
	button_prev.handleEvent(e, m_pGame->getWindow(), *this);
}

void UIState::handleInput() 
{
}

void UIState::update(sf::Time deltaTime) 
{
    if (is_pause == false)
    {
        cnt_loop++;
        if (cnt_loop > 60 * 1) // 1 sec
        {
            index_img++;
            if (index_img >= img_sprite.size() - 1)
            {
                index_img = 0;
            }
            cnt_loop = 0;
        }
    }
}

void UIState::fixedUpdate(sf::Time deltaTime) 
{

}

void UIState::render(sf::RenderTarget& renderer) 
{
    refresh_size();

    if (img_sprite.size() > 0)
    {
        sprite_canva = img_sprite[index_img];
        if (img_sprite[index_img].get() != nullptr)
        {
            sprite_canva.reset();
            sprite_canva = std::shared_ptr<sf::Sprite>(new sf::Sprite(*img_texture[index_img].get()));
 
            sprite_canva->scale(scale(sprite_canva));
            renderer.draw(*(sprite_canva.get()));
        }
    }

    button_pause.render(renderer);
    button_name.render(renderer);
    button_parts.render(renderer);
	button_prev.render(renderer);
}

void UIState::refresh_size()
{
    w = (int) ui.getWindow().getSize().x;
    h = (int) ui.getWindow().getSize().y;
    canvas_w = (int)(canvas_x_perc * w);
    canvas_h = (int)(canvas_y_perc * h);
    left_w = (int)((1.0f - canvas_x_perc) * w);
    left_h = (int)((1.0f - canvas_y_perc) * h);

    button_pause.setPosition({ (float)canvas_w, 10 });

	button_prev.setText("prev");
	button_prev.setPosition({ (float)canvas_w, 100 });

    button_name.setPosition({ (float)10, (float)canvas_h });
    button_parts.setPosition({ (float)300, (float)canvas_h });
}

sf::Vector2f UIState::scale(std::shared_ptr<sf::Sprite> sprite)
{
    float sx = ((float)canvas_w) / (float)sprite->getTextureRect().width;
    float sy = ((float)canvas_h) / (float)sprite->getTextureRect().height;
    return sf::Vector2f{ std::min(sx, sy), std::min(sx, sy) };
}

void UIState::load_path(filesystem::path& p)
{
    ini_filename.clear();
    img_files.clear();
    img_texture.clear();
    img_sprite.clear();

    std::vector<std::string> files = filesystem::path::get_directory_file(p, false);
    for (size_t i = 0; i < files.size(); i++)
    {
        filesystem::path pv = files.at(i);
        if (pv.is_file())
        {
            std::string s = pv.extension();
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            if ((s== "jpg") || (s == "png") || (s == "gif") || (s == "jpeg"))
            {
                img_files.push_back(pv);
            }

            if (pv.extension() == "ini")
            {
                if (pv.filename() == "desc.ini")
                {
                    ini.reset();
                    ini = std::shared_ptr<ini_parser>(new ini_parser(pv.make_absolute().str()));

                    std::string name = ini->get_string("name", "main");

                    std::string s = ini->get_string("edible", "parts");
                    std::vector<std::string> edible_parts = split(s, ';');

                    button_name.setText(name);
                    s = "Edible: " + merge(edible_parts);
                    button_parts.m_button.setSize({ (float) (15 * s.size()) , 64 });
                    button_parts.setText(s);
                }
            }
        }
    }

    refresh_size();
    for (size_t i = 0; i < img_files.size(); i++)
    {
        std::shared_ptr<sf::Texture> texture(new sf::Texture);
        texture->loadFromFile(img_files[i].make_absolute().str());
        img_texture.push_back(texture);

        std::shared_ptr<sf::Sprite> sprite(new sf::Sprite(*texture));
        img_sprite.push_back(sprite);
    }
}
