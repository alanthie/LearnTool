//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
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

std::string ExePath() 
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

//-----------------------------------------
// Argument: Config file path
//
// Ex: LearnTool.exe .\\LearnTool.ini
//-----------------------------------------
int main(int argc, char *argv[])
{
    Config cfg; // has defaults values

    if (argc >= 2)
    {
        std::string config_file = std::string(argv[1]);

        if (cfg.setup(config_file) == true)
        {
			
        }
        else
        {
			std::cerr << "Invalid config file." << std::endl;
			return -1;
        }
    }
    else
    {
        std::string exe_path = ExePath();
        std::cout << "Looking for config file: " + exe_path << "\\LearnTool.ini" << std::endl;

        if (cfg.setup(exe_path + "\\LearnTool.ini") == true)
        {

        }
        else
        {
            std::cerr << "Invalid config LearnTool.ini file." << std::endl;
            return -1;
        }
    }

   

    std::cout << "Ressource Path:" << cfg.path_dir << std::endl;

    UImain ui(cfg);
    ui.run();
    return 0;
}