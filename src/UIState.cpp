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
#include <cassert>

void UIState::b_click(std::string& b_name)
{
    std::cout << "button" << b_name << " clicked!" << '\n';
    if (b_name == "b_pause")
    {
        is_pause = !is_pause;
		if (is_pause)
            button_menu[0][0]->setText("continue");
		else
            button_menu[0][0]->setText("pause");
    }

    else if (b_name == "b_img_next")
    {
        index_img++;
        if (index_img > img_sprite.size() - 1)
        {
            index_img = 0;
        }
    }

    else if (b_name == "b_img_prev")
    {
        index_img--;
        if (index_img < 0)
        {
            index_img = (long)img_sprite.size() - 1;
        }
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
    button_name("b_name",   gui::ButtonSize::Small),
    button_parts("b_parts", gui::ButtonSize::Wide)
{
    button_menu[0][0] = new gui::Button("b_pause", gui::ButtonSize::Small);
    button_menu[1][0] = new gui::Button("b_img_prev", gui::ButtonSize::Small);
    button_menu[1][1] = new gui::Button("b_img_next", gui::ButtonSize::Small);
    button_menu[2][0] = new gui::Button("b_zoom_plus", gui::ButtonSize::Small);
    button_menu[2][1] = new gui::Button("b_zoom_less", gui::ButtonSize::Small);
    button_menu[3][0] = new gui::Button("b_topic_prev", gui::ButtonSize::Small);
    button_menu[3][1] = new gui::Button("b_topic_next", gui::ButtonSize::Small);

    button_menu[0][0]->setText("pause");
    button_menu[1][0]->setText("<");
    button_menu[1][1]->setText(">");
    button_menu[2][0]->setText("+");
    button_menu[2][1]->setText("-");
    button_menu[3][0]->setText("<<");
    button_menu[3][1]->setText(">>");
    
    float b_w = button_menu[0][0]->m_text.getLocalBounds().width;
    button_menu[0][0]->m_rect.setSize({ 2 * b_w , b_h });
    button_menu[1][0]->m_rect.setSize({ b_w , b_h });
    button_menu[1][1]->m_rect.setSize({ b_w , b_h });
    button_menu[2][0]->m_rect.setSize({ b_w , b_h });
    button_menu[2][1]->m_rect.setSize({ b_w , b_h });
    button_menu[3][0]->m_rect.setSize({ b_w , b_h });
    button_menu[3][1]->m_rect.setSize({ b_w , b_h });

    button_menu[0][0]->setFunction(&StateBase::b_click);
    button_menu[1][0]->setFunction(&StateBase::b_click);
    button_menu[1][1]->setFunction(&StateBase::b_click);
    button_menu[2][0]->setFunction(&StateBase::b_click);
    button_menu[2][1]->setFunction(&StateBase::b_click);
    button_menu[3][0]->setFunction(&StateBase::b_click);
    button_menu[3][1]->setFunction(&StateBase::b_click);
    button_name.setFunction(    &StateBase::b_click);
    button_parts.setFunction(   &StateBase::b_click);

    root = filesystem::path("..\\res\\topic");
    root_files = filesystem::path::get_directory_file(root, false);

    current_parent = filesystem::path(root);
    current_path = find_next_folder(root, filesystem::path());
    load_path(current_path);
    /*load_path(filesystem::path("..\\res\\\plant\\chufa (leave_stem, oil, root)"));*/
}

void UIState::end_path()
{
    current_path = find_next_folder(current_parent, current_path);
    if (current_path.empty() == false)
    {
        load_path(current_path);
    }
    else
    {
        current_parent = filesystem::path(root);
        current_path = find_next_folder(root, filesystem::path());
        load_path(current_path);
    }
}

filesystem::path UIState::find_next_folder(filesystem::path parent_folder, filesystem::path last_folder)
{
    filesystem::path p;
    if (last_folder.empty())
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false);
        for (size_t i = 0; i < v.size(); i++)
        {
            p = filesystem::path(v[i]);
            break;
        }
    }
    else
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false);
        for (size_t i = 0; i < v.size(); i++)
        {
            if (v[i] == last_folder.make_absolute().str())
            {
                if (i < v.size() - 1)
                {
                    p = filesystem::path(v[i + 1]);
                    break;
                }
            }
        }
    }

    return p;
}

void UIState::handleEvent(sf::Event e) 
{
    button_menu[0][0]->handleEvent(e, m_pGame->getWindow(), *this);
    button_menu[1][0]->handleEvent(e, m_pGame->getWindow(), *this);
    button_menu[1][1]->handleEvent(e, m_pGame->getWindow(), *this);
    button_menu[2][0]->handleEvent(e, m_pGame->getWindow(), *this);
    button_menu[2][1]->handleEvent(e, m_pGame->getWindow(), *this);
    button_menu[3][0]->handleEvent(e, m_pGame->getWindow(), *this);
    button_menu[3][1]->handleEvent(e, m_pGame->getWindow(), *this);

    button_name.handleEvent(e, m_pGame->getWindow(), *this);
    button_parts.handleEvent(e, m_pGame->getWindow(), *this);
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
            if (index_img > img_sprite.size() - 1)
            {
                end_path();
                //index_img = 0;
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
        assert(index_img >= 0);
        assert(index_img <= img_sprite.size() - 1);

        sprite_canva = img_sprite[index_img];
        if (img_sprite[index_img].get() != nullptr)
        {
            sprite_canva.reset();
            sprite_canva = std::shared_ptr<sf::Sprite>(new sf::Sprite(*img_texture[index_img].get()));
 
            sprite_canva->scale(scale(sprite_canva));
            renderer.draw(*(sprite_canva.get()));
        }
    }

    button_menu[0][0]->render(renderer);
    button_menu[1][0]->render(renderer);
    button_menu[1][1]->render(renderer);
    button_menu[2][0]->render(renderer);
    button_menu[2][1]->render(renderer);
    button_menu[3][0]->render(renderer);
    button_menu[3][1]->render(renderer);

    button_name.render(renderer);
    button_parts.render(renderer);
}

void UIState::refresh_size()
{
    w = (int) ui.getWindow().getSize().x;
    h = (int) ui.getWindow().getSize().y;
    canvas_w = (int)(canvas_x_perc * w);
    canvas_h = (int)(canvas_y_perc * h);
    left_w = (int)((1.0f - canvas_x_perc) * w);
    left_h = (int)((1.0f - canvas_y_perc) * h);
    float b_w = (float)(w - canvas_w) / 2;

    button_menu[0][0]->setPosition({ (float)canvas_w, 0 });
    button_menu[0][0]->m_rect.setSize({ 2 * b_w , b_h });

    button_menu[1][0]->m_rect.setSize({ b_w , b_h });
    button_menu[1][1]->m_rect.setSize({ b_w , b_h });
    button_menu[2][0]->m_rect.setSize({ b_w , b_h });
    button_menu[2][1]->m_rect.setSize({ b_w , b_h });
    button_menu[3][0]->m_rect.setSize({ b_w , b_h });
    button_menu[3][1]->m_rect.setSize({ b_w , b_h });

    button_menu[1][0]->setPosition({ (float)canvas_w, b_h });
    button_menu[1][1]->setPosition({ (float)canvas_w + b_w, b_h });
    button_menu[2][0]->setPosition({ (float)canvas_w, 2*b_h });
    button_menu[2][1]->setPosition({ (float)canvas_w + b_w, 2*b_h });
    button_menu[3][0]->setPosition({ (float)canvas_w, 3 * b_h });
    button_menu[3][1]->setPosition({ (float)canvas_w + b_w, 3 * b_h });

    button_name.setPosition({ (float)0, (float)canvas_h });
    button_name.m_rect.setSize({ button_name.getSize().x  , b_h });

    button_parts.setPosition({ button_name.getSize().x , (float)canvas_h });
    button_parts.m_rect.setSize({ w - (0 + button_name.getSize().x )  , b_h });
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

    index_img = 0;
    cnt_loop = 0;

    std::vector<std::string> files = filesystem::path::get_directory_file(p, false);
    for (size_t i = 0; i < files.size(); i++)
    {
        filesystem::path pv = files.at(i);
        if (pv.is_file())
        {
            std::string s = pv.extension();
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            if ((s== "jpg") || (s == "png") || (s == "gif") || (s == "jpeg") || (s == "bmp"))
            {
                img_files.push_back(pv);
            }

            if (pv.extension() == "ini")
            {
                if (pv.filename() == "desc.ini")
                {
                    ini.reset();
                    ini = std::shared_ptr<ini_parser>(new ini_parser(pv.make_absolute().str()));

                    std::string name;
                    try
                    {
                        name = ini->get_string("name", "main");
                    }
                    catch (...)
                    {
                    }

                    std::string key;
                    try
                    {
                        key = ini->get_string("key", "parts");
                    }
                    catch (...)
                    {
                    }

                    std::string value;
                    try
                    {
                        value = ini->get_string("value", "parts");
                    }
                    catch (...)
                    {
                    }

                    std::string desc;
                    if (key.size() > 0)
                    {
                        std::vector<std::string> parts = split(value, ';');
                        desc = key + ": " + merge(parts);
                    }

                    button_name.setText(name);

                    button_parts.setText(desc);
                    button_parts.m_rect.setSize({ 50 + (float)(button_parts.m_text.getLocalBounds().width) , b_h });
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
