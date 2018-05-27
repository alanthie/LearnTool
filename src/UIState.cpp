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

void UIState::img_changed()
{
    minimap.moving = false;
    minimap.ratio_offset = { 0.0f, 0.0f };
    const sf::Vector2f pos_rect = minimap.m_rect.getPosition();
    const sf::Vector2f wh_rect = minimap.m_rect.getSize();
    minimap.m_drag_rect.setPosition(pos_rect.x + 1, pos_rect.y + 1);
    minimap.m_drag_rect.setSize({ wh_rect.x - 1, wh_rect.y - 1 });

    canvas_scale = { 1.0f, 1.0f };
}

void UIState::minmap_change(std::string& b_name) 
{
    std::cout << "minmap" << b_name << " change!" << '\n';
    const sf::Vector2f pos = minimap.m_drag_rect.getPosition();
    const sf::Vector2f pos_rect = minimap.m_rect.getPosition();
    const sf::Vector2f wh = minimap.m_drag_rect.getSize();
    const sf::Vector2f delta_pos = (pos - pos_rect) ;
    minimap.ratio_offset = sf::Vector2f{ 0.0f + (delta_pos.x/wh.x), 0.0f + (delta_pos.y / wh.y) };
}

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
        if (index_img > img_files.size() - 1)
        {
            index_img = 0;
        }
        img_changed();
    }
    else if (b_name == "b_img_prev")
    {
        index_img--;
        if (index_img < 0)
        {
            index_img = (long)img_files.size() - 1;
        }
        img_changed();
    }

    else if (b_name == "b_topic_prev")
    {
        prev_path();
        img_changed();
    }
    else if (b_name == "b_topic_next")
    {
        next_path();
        img_changed();
    }

    else if (b_name == "b_zoom_plus")
    {
        canvas_scale = canvas_scale * 1.25f;
        minimap.set_view((float)canvas_w, (float)canvas_h, canvas_full_scale * 1.25f , canvas_movepos);
    }
    else if (b_name == "b_zoom_less")
    {
        canvas_scale = canvas_scale / 1.25f;
        minimap.set_view((float)canvas_w, (float)canvas_h, canvas_full_scale / 1.25f , canvas_movepos);
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
    button_parts("b_parts", gui::ButtonSize::Wide),
    button_msg("b_msg",     gui::ButtonSize::Wide),
    minimap("mmap", 50, 50)
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

    minimap.m_rect.setSize({ 2 * b_w , 2 * b_w, });

    button_menu[0][0]->setFunction(&StateBase::b_click);
    button_menu[1][0]->setFunction(&StateBase::b_click);
    button_menu[1][1]->setFunction(&StateBase::b_click);
    button_menu[2][0]->setFunction(&StateBase::b_click);
    button_menu[2][1]->setFunction(&StateBase::b_click);
    button_menu[3][0]->setFunction(&StateBase::b_click);
    button_menu[3][1]->setFunction(&StateBase::b_click);
    button_name.setFunction(    &StateBase::b_click);
    button_parts.setFunction(   &StateBase::b_click);

    minimap.setFunction(&StateBase::minmap_change);

    root = filesystem::path("..\\res\\topic");
    //root = filesystem::path("E:\\000 plant\\p");
    //root = filesystem::path("E:\\000 plant\\p root");
    root_files = filesystem::path::get_directory_file(root, false, true);

    current_parent  = filesystem::path(root);
    current_path    = find_next_folder(root, filesystem::path());

    if (current_path.empty() == false)
    {
        load_path(current_path);
    }
}

void UIState::load_root()
{
    // Restart
    current_parent = filesystem::path(root);
    current_path = find_next_folder(root, filesystem::path());

    if (current_path.empty() == false)
    {
        load_path(current_path);
    }
    else
    {
        assert(false);
    }
}

void UIState::next_path(bool no_deepening)
{
    filesystem::path save_current_path = current_path;
    //filesystem::path save_current_parent = current_parent;
    filesystem::path save_current_parent = save_current_path.parent_path();

    assert(save_current_parent.empty() == false);

    current_path = find_next_folder(save_current_parent, save_current_path, no_deepening);
    if (current_path.empty() == false)
    {
        if (current_parent != current_path.parent_path())
        {
            current_parent = current_path.parent_path();
        }

        if (current_path.empty() == false)
        {
            load_path(current_path);
        }
        else
        {
            assert(false);
        }
    }
    else if (save_current_parent.make_absolute().str() == root.make_absolute().str())
    {
         // Restart
        load_root();
        return;
    }
    else
    {
        current_path = save_current_parent;
        current_parent = save_current_parent.parent_path();
        if (current_path.make_absolute().str() == root.make_absolute().str())
        {
            // Restart
            load_root();
            return;
        }

        current_path = find_next_folder(current_parent, current_path, true);
        while (current_path.empty() == true)
        {
            current_path = save_current_parent.parent_path();
            current_parent = save_current_parent.parent_path().parent_path();

            if (current_path.make_absolute().str() == root.make_absolute().str())
            {
                // Restart
                load_root();
                return;
            }

            next_path(true);
        }
        
        if (current_path.empty() == false)
        {
            load_path(current_path);
        }
        else
        {
            assert(false);
        }
    }
}

void UIState::prev_path(bool no_deepening)
{
    filesystem::path save_current_path = current_path;
    /*filesystem::path save_current_parent = current_parent;*/
    filesystem::path save_current_parent = save_current_path.parent_path();

    current_path = find_prev_folder(save_current_parent, save_current_path, no_deepening);
    if (current_path.empty() == false)
    {
        current_parent = current_path.parent_path();
        if (current_path.make_absolute().str() == root.make_absolute().str())
        {
            // Restart
            load_root();
            return;
        }
        else
        {
            if (current_path.empty() == false)
            {
                load_path(current_path);
            }
            else
            {
                assert(false);
            }
        }
    }
    else
    {
        current_path = save_current_parent;
        current_parent = save_current_parent.parent_path();

        if ((current_path.empty() == true) || (current_path.make_absolute().str() == root.make_absolute().str()))
        {
            // Restart
            load_root();
            return;
        }

        if (current_path.empty() == false)
        {
            current_parent = current_path.parent_path();
            load_path(current_path);
        }
        else
        {
            assert(false);
        }
    }
}

filesystem::path UIState::find_next_folder(filesystem::path parent_folder, filesystem::path last_folder, bool no_deepening)
{
    filesystem::path p;
    if (last_folder.empty())
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
        for (size_t i = 0; i < v.size(); i++)
        {
            if (filesystem::path(v[i]).filename() != ".Thumbs")
            {
                std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
                if (vf.size() > 0)
                {
                    p = filesystem::path(v[i]);
                    break;
                }
            }
        }
    }
    else
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
        int k = 0;
        for (int i = 0; i < v.size(); i++)
        {
            if (v[i] == last_folder.make_absolute().str())
            {
                k = i;
                break;
            }
        }

        if (no_deepening == false)
        {
            std::vector<std::string> v_sub = filesystem::path::get_directory_file(last_folder, false, true);
            for (size_t j = 0; j < v_sub.size(); j++)
            {
                if (filesystem::path(v_sub[j]).filename() != ".Thumbs")
                {
                    std::vector<std::string> vf_sub = get_img_files(filesystem::path(v_sub[j]));
                    if (vf_sub.size() > 0)
                    {
                        p = filesystem::path(v_sub[j]);
                        return p;
                    }
                }
            }
        }

        for (int i = k+1; i < v.size(); i++)
        {
            if (filesystem::path(v[i]).filename() != ".Thumbs")
            {
                std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
                if (vf.size() > 0)
                {
                    p = filesystem::path(v[i]);
                    break;
                }
            }
        }

    }

    return p;
}

filesystem::path UIState::find_last_folder(filesystem::path parent_folder)
{
    filesystem::path p;

    std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
    for (size_t i = v.size() - 1; i >= 0; i--)
    { 
        if (filesystem::path(v[i]).filename() != ".Thumbs")
        {
            std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
            if (vf.size() > 0)
            {
                p = filesystem::path(v[i]);
                break;
            }
        }
    }
    return p;
}

filesystem::path UIState::find_prev_folder(filesystem::path parent_folder, filesystem::path last_folder, bool no_deepening)
{
    filesystem::path p;
    if (last_folder.empty())
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
        for (size_t i = 0; i < v.size(); i++)
        {
            if (filesystem::path(v[i]).filename() != ".Thumbs")
            {
                std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
                if (vf.size() > 0)
                {
                    p = filesystem::path(v[i]);
                    break;
                }
            }
        }
    }
    else
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
        int k = 0;
        for (int i = 0; i < v.size(); i++)
        {
            if (v[i] == last_folder.make_absolute().str())
            {
                k = i;
                break;
            }
        }

        for (int i = k-1; i >= 0; i--)
        {
            if (filesystem::path(v[i]).filename() != ".Thumbs")
            {
                std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
                if (vf.size() > 0)
                {
                    p = filesystem::path(v[i]);
                    break;
                }
            }
        }
        
        // at root
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

    button_name.handleEvent(e,  m_pGame->getWindow(), *this);
    button_parts.handleEvent(e, m_pGame->getWindow(), *this);
    button_msg.handleEvent(e,   m_pGame->getWindow(), *this);

    minimap.handleEvent(e, m_pGame->getWindow(), *this);
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
            if (index_img > img_files.size() - 1)
            {
                next_path();
            }
            cnt_loop = 0;
            img_changed();
        }
    }
}

void UIState::fixedUpdate(sf::Time deltaTime) 
{

}

void UIState::render(sf::RenderTarget& renderer) 
{
    refresh_size();

    if (img_files.size() > 0)
    {
        assert(index_img >= 0);
        assert(index_img <= img_files.size() - 1);

        if (img_texture[index_img].get() == nullptr)
        {
            img_texture[index_img] = std::shared_ptr<sf::Texture>(new sf::Texture);
            img_texture[index_img]->loadFromFile(img_files[index_img].make_absolute().str());
        }

        if (img_texture[index_img].get() != nullptr)
        {
            sprite_canva.reset();
            sprite_canva = std::shared_ptr<sf::Sprite>(new sf::Sprite(*img_texture[index_img].get()));
            sprite_canva->scale(scale(sprite_canva));
            sprite_canva->scale(canvas_scale);

            canvas_full_scale = sprite_canva->getScale();

            sprite_canva->move(-1.0f * minimap.ratio_offset.x * (float)canvas_w, -1.0f * minimap.ratio_offset.y * (float)canvas_h);
            canvas_movepos = { -1.0f * minimap.ratio_offset.x * (float)canvas_w, -1.0f * minimap.ratio_offset.y * (float)canvas_h };
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

    minimap.render(renderer);

    button_name.render(renderer);
    button_parts.render(renderer);
    button_msg.render(renderer);
}

void UIState::refresh_size()
{
    w = (int) ui.getWindow().getSize().x;
    h = (int) ui.getWindow().getSize().y;
    canvas_w = (int)(canvas_x_perc * w);
    canvas_h = (int)(h - 2 * b_h);
    float b_w = (float)(w - canvas_w -1) / 2;

    button_menu[0][0]->setPosition({ (float)canvas_w, 1 });
    button_menu[0][0]->m_rect.setSize({ 2 * b_w  , b_h });

    button_menu[1][0]->m_rect.setSize({ b_w , b_h });
    button_menu[1][1]->m_rect.setSize({ b_w  , b_h });
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

    //float mmap_w = 4 * b_h - 1;
    float mmap_w = 2 * b_w;
    minimap.m_rect.setSize({ mmap_w , mmap_w, });
    if (minimap.moving == false)
    {
        //minimap.setPosition({ (float)canvas_w - mmap_w, 1 });
        minimap.setPosition({ (float)canvas_w, 4 * b_h });
    }

    button_name.setPosition({ (float)1, (float)canvas_h });
    button_name.m_rect.setSize({ (float)((button_parts.m_text.getString().getSize() == 0)? w-1: w/3 ) , b_h });

    button_parts.setPosition({ button_name.getSize().x , (float)canvas_h });
    button_parts.m_rect.setSize({ w - (button_name.getSize().x + 1) , b_h });

    button_msg.setPosition({ 1 , (float)canvas_h + b_h });
    button_msg.m_rect.setSize({ (float)w - 2, b_h - 1 });
}

sf::Vector2f UIState::scale(std::shared_ptr<sf::Sprite> sprite)
{
    float sx = ((float)canvas_w) / (float)sprite->getTextureRect().width;
    float sy = ((float)canvas_h) / (float)sprite->getTextureRect().height;
    return sf::Vector2f{ std::min(sx, sy), std::min(sx, sy) };
}

std::vector<std::string> UIState::get_img_files(filesystem::path& p)
{
    std::vector<std::string> imgfiles;
    std::vector<std::string> files = filesystem::path::get_directory_file(p, false);
    for (size_t i = 0; i < files.size(); i++)
    {
        filesystem::path pv = files.at(i);
        if (pv.is_file())
        {
            std::string s = pv.extension();
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            if ((s == "jpg") || (s == "png") || (s == "gif") || (s == "jpeg") || (s == "bmp"))
            {
                imgfiles.push_back(pv.make_absolute().str());
            }
        }
    }
    return imgfiles;
}

void UIState::load_path(filesystem::path& p)
{
    ini_filename.clear();
    img_files.clear();
    img_texture.clear();

    index_img = 0;
    cnt_loop = 0;
    ini.reset();

    {
        button_msg.setText("loading...");
        button_name.setText("");
        button_parts.setText("");

        ui.getWindow().clear();
        render(ui.getWindow());
        ui.getWindow().display();
    }

    std::string fullname = p.make_absolute().str();
    std::string name = fullname.substr(fullname.find(root.make_absolute().str()) + root.make_absolute().str().size()+1);
    std::string desc;

    std::cout << "Name:" << name << std::endl;

    ui.getWindow().setTitle(ui.cfg.title + " [" + p.make_absolute().str() + "]");

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
                    ini = std::shared_ptr<ini_parser>(new ini_parser(pv.make_absolute().str()));

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

                    if (key.size() > 0)
                    {
                        std::vector<std::string> parts = split(value, ';');
                        desc = key + ": " + merge(parts);
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < img_files.size(); i++)
    {
        std::shared_ptr<sf::Texture> texture(nullptr);
        img_texture.push_back(texture);
    }

    button_name.setText(name);
    button_parts.setText(desc);
    button_msg.setText("");
}
