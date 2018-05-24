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

//#include "../src/filesystem/path.h"
//#include "../src/filesystem/resolver.h"

#include "UImain.h"
#include "Config.hpp"
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>

int main(int argc, char *argv[])
{
    Config cfg;
    UImain ui(cfg.default_w, cfg.default_h, cfg.title);

    ui.run();

    return 0;
}