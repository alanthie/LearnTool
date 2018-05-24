//

#include "UIMain.h"
#include "UIState.h"
#include "SFML_SDK/Game.h"
#include <string>

UImain::UImain(int w, int h, std::string title) : Game(w, h, title)
{
    pushState<UIState>(*this);
}

