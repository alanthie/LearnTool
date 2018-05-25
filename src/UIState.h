//
//
#pragma once

#include "UIMain.h"
#include "SFML_SDK//GUI/Button.h"
#include "SFML_SDK//GUI/Textbox.h"
#include "SFML_SDK/States/StateBase.h"
#include "SFML_SDK/GUI/StackMenu.h"
#include "SFML_SDK/GUI/Button.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics.hpp>
#include "filesystem/path.h"
#include "filesystem/resolver.h"
#include "ini_parser/ini_parser.hpp"
#include <memory>
#include <iostream>

//
class UIState : public StateBase
{
public:
    UImain&                     ui;

    gui::Button*                button_menu[5][2] = { {nullptr}, { nullptr },{ nullptr } ,{ nullptr } ,{ nullptr } };
    gui::Button                 button_name;
    gui::Button                 button_parts;
    std::shared_ptr<sf::Sprite> sprite_canva;

    bool is_pause = false;

    filesystem::path            root;
    std::vector<std::string>    root_files;

    filesystem::path            current_parent;
    filesystem::path            current_path;

    std::string                 ini_filename;
    std::shared_ptr<ini_parser> ini;

    long                        index_img = 0;
    long                        cnt_loop = 0;

    std::vector<filesystem::path>               img_files;
    std::vector<std::shared_ptr<sf::Texture>>   img_texture;
    std::vector<std::shared_ptr<sf::Sprite>>    img_sprite;

    float canvas_x_perc = 0.85f;
    float canvas_y_perc = 0.85f;
    int w;
    int h;
    int canvas_w;
    int canvas_h;
    int left_w;
    int left_h;
    float b_h = 50;

public:
    UIState(UImain& g);

    void handleEvent(sf::Event e) override;
    void handleInput() override;
    void update(sf::Time deltaTime) override;
    void fixedUpdate(sf::Time deltaTime) override;
    void render(sf::RenderTarget& renderer) override;

    void            refresh_size();
    sf::Vector2f    scale(std::shared_ptr<sf::Sprite> sprite);
    void            load_path(filesystem::path& p);

    void b_click(std::string& b_name) override;

    filesystem::path find_next_folder(filesystem::path parent_folder, filesystem::path last_folder);

    void end_path();
};
