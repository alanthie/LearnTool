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
#include "FolderNavigation.h"
#include "VideoCapture.hpp"

enum class display_mode {show_img, show_movie};

class UIState : public StateBase
{
public:
    UImain&                     ui;
    sf::View                    main_view;
    display_mode                _mode = display_mode::show_img;
    FolderNavigation            _fnav;

    VideoCapturing*                     _vc = nullptr;  // current
    std::vector<VideoCapturing*>        v_vc;           // preload

    std::vector<VideoCapturingDeleter*>  v_vcd;
    std::vector<ExtractSound*>           v_extract_sound;

    gui::Button*                button_menu[6][2] = { {nullptr}, { nullptr },{ nullptr } ,{ nullptr } ,{ nullptr } ,{ nullptr } };
    gui::Button                 button_name;
    gui::Button                 button_parts;
    gui::Button                 button_msg;
    std::shared_ptr<sf::Sprite> sprite_canva;
    gui::Minimap                minimap;
    sf::View                    view_minimap;

    bool                        is_pause = false;

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

    void            recalc_size();
    sf::Vector2f    scale_sprite(std::shared_ptr<sf::Sprite> sprite);
    sf::Vector2f    canvas_scale = { 1.0f, 1.0f };

    void load_path(filesystem::path& p);

    void widget_clicked(std::string& b_name) override;
    void widget_changed(std::string& b_name) override;
};
