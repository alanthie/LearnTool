//
#pragma once

#include "SFML_SDK/Game.h"
#include "Config.hpp"
#include <string>

class UImain : public Game
{
public:
    UImain(Config& _cfg);

    Config cfg;
};
