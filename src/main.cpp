//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>

#include "filesystem/path.h"
#include "filesystem/resolver.h"
#include "ini_parser/ini_parser.hpp"

#include "UImain.h"
#include "Config.hpp"
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>

//-----------------------------------------
// Argument: Config file path
//
// Ex: LearnTool.exe .\\LearnTool.ini
//-----------------------------------------
// Contents of LearnTool.ini:
//
//  [main]
//  path_folder=E:\000 plant
//  title=Plant database
//  w=1024
//  h=800
//  zoom=1.50
//  exclude_folder=.Thumbs;.dummy
//  img=jpg;png;gif;jpeg;bmp
//-----------------------------------------

int main(int argc, char *argv[])
{
    Config cfg; // has defaults values

    if (argc >= 2)
    {
        std::string config_file = std::string(argv[1]);
        if (config_file.size() > 0)
        {
            filesystem::path p(config_file);
            if ((p.empty() == false) && (p.exists() == true) && (p.is_file() == true))
            {
                std::shared_ptr<ini_parser> cfg_ini = std::shared_ptr<ini_parser>(new ini_parser(p.make_absolute().str()));
                if (cfg_ini)
                {
                    std::string path_dir_temp;
                    try
                    {
                        path_dir_temp = cfg_ini->get_string("path_folder", "main");
                        filesystem::path path_folder(path_dir_temp);
                        if ((path_folder.empty() == false) && (path_folder.exists() == true) && (path_folder.is_directory() == true))
                        {
                            cfg.path_dir = path_folder.make_absolute().str();
                        }
                    }
                    catch (...)
                    {
                    }

                    try
                    {
                        cfg.title = cfg_ini->get_string("title", "main");
                    }
                    catch (...)
                    {
                    }

                    try
                    {
                        cfg.default_w = cfg_ini->get_int("w", "main");
                    }
                    catch (...)
                    {
                    }

                    try
                    {
                        cfg.default_h = cfg_ini->get_int("h", "main");
                    }
                    catch (...)
                    {
                    }

                    try
                    {
                        cfg.zoom = std::max(1.05f, cfg_ini->get_float("zoom", "main") );
                    }
                    catch (...)
                    {
                    }

                    try
                    {
                        std::string s_excl = cfg_ini->get_string("exclude_folder", "main");
                        cfg.exclude_folder = Config::split(s_excl, ';');
                    }
                    catch (...)
                    {
                    }

                    try
                    {
                        std::string s_img = cfg_ini->get_string("img", "main");
                        cfg.img = Config::split(s_img, ';');
                    }
                    catch (...)
                    {
                    }
                }
            }
        }
    }

    filesystem::path path_folder(cfg.path_dir);
    if ((path_folder.empty() == false) && (path_folder.exists() == true) && (path_folder.is_directory() == true))
    {
        // ok
    }
    else
    {
        std::cerr << "Invalid config file." << std::endl;
        return -1;
    }

    std::cerr << "Ressource Path:" << path_folder.make_absolute().str() << std::endl;

    UImain ui(cfg);
    ui.run();
    return 0;
}