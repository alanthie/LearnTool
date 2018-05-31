//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#include "UIMain.h"
#include "UIState.h"
#include "SFML_SDK/Game.h"
#include <string>

UImain::UImain(Config& _cfg) : 
    Game(_cfg.default_w, _cfg.default_h, _cfg.title), 
    cfg(_cfg)
{
    pushState<UIState>(*this);
}

