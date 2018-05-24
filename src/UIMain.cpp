//

#include "UIMain.h"
#include "UIState.h"
#include "Game.h"
#include <string>

UImain::UImain(int w, int h, std::string title) : Game(w, h, title)
{
    pushState<UIState>(*this);
}

