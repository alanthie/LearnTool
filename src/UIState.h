//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#pragma once

#include "UIMain.h"
#include "SFML_SDK//GUI/Button.h"
#include "SFML_SDK//GUI/Textbox.h"
#include "SFML_SDK/States/StateBase.h"
#include "SFML_SDK/GUI/StackMenu.h"
#include "SFML_SDK/GUI/Button.h"
#include "SFML_SDK/GUI/Minimap.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics.hpp>
#include "filesystem/path.h"
#include "filesystem/resolver.h"
#include "ini_parser/ini_parser.hpp"
#include "VideoCapture.hpp"
#include <memory>
#include <iostream>

//
enum class display_mode {show_img, show_movie};

class UIState : public StateBase
{
public:
    UImain&                     ui;
    sf::View                    main_view;
    display_mode                _mode = display_mode::show_img;

    //sf::SoundBuffer buffer;
    //sf::Sound sound;

    VideoCapturing*             _vc = nullptr;

    gui::Button*                button_menu[5][2] = { {nullptr}, { nullptr },{ nullptr } ,{ nullptr } ,{ nullptr } };
    gui::Button                 button_name;
    gui::Button                 button_parts;
    gui::Button                 button_msg;
    std::shared_ptr<sf::Sprite> sprite_canva;
    gui::Minimap                minimap;
    sf::View                    view_minimap;

    bool                        is_pause = false;

    filesystem::path            root;
    std::vector<std::string>    root_files;
    filesystem::path            current_parent;
    filesystem::path            current_path;

    std::string                 ini_filename;
    std::shared_ptr<ini_parser> ini;

    long                                        index_img = 0;
    long                                        cnt_loop = 0;
	float							            vitesse_img_sec         = 3.0f;         // SEC
    float							            vitesse_video_factor    = 1.0f;
    std::vector<filesystem::path>               img_files;
    std::vector<std::shared_ptr<sf::Texture>>   img_texture;

    float canvas_x_perc = 0.85f;
    float w;
    float h;
    float canvas_w;
    float canvas_h;
    float b_h = 50;
    sf::FloatRect canvas_bounds;

public:
    UIState(UImain& g);

    void img_changed();

    void handleEvent(sf::Event e) override;
    void handleInput() override;
    void update(sf::Time deltaTime) override;
    void fixedUpdate(sf::Time deltaTime) override;
    void render(sf::RenderTarget& renderer) override;

    void            refresh_size();
    sf::Vector2f    scale(std::shared_ptr<sf::Sprite> sprite);
    sf::Vector2f    canvas_scale = { 1.0f, 1.0f };

    void load_path(filesystem::path& p);
    void load_root();

    void b_click(std::string& b_name) override;
    void minmap_change(std::string& b_name) override;

    filesystem::path find_next_folder(filesystem::path parent_folder, filesystem::path last_folder, bool no_deepening = false);
    filesystem::path find_prev_folder(filesystem::path parent_folder, filesystem::path last_folder, bool no_deepening = false);
    filesystem::path find_last_folder(filesystem::path parent_folder);

    void next_path(bool no_deepening = false);
    void prev_path(bool no_deepening = false);

    std::vector<std::string> get_img_files(filesystem::path& p);
};
