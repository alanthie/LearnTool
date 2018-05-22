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

#include "../src/filesystem/path.h"
#include "../src/filesystem/resolver.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>

using namespace std;
using namespace sf;
using namespace filesystem;

int main(int argc, char *argv[])
{
    std::vector<path> files_img;

    path p = path("..\\chufa (leave_stem, oil, root)");
    std::vector<std::string> files = filesystem::get_directory_file(p, false);
    for (size_t i = 0; i < files.size(); i++)
    {
        path pv = files.at(i);
        if (pv.is_file())
        {
            if ((pv.extension() == "jpg") || (pv.extension() == "png") || (pv.extension() == "gif") || (pv.extension() == "jpeg"))
            {
                files_img.push_back(pv);
            }
        }
    }

    if (files_img.size() == 0)
    {
        return 0;
    }

    try
    {
        // Fenêtre
        RenderWindow Fenetre(VideoMode(600, 600), "Learning Tool");

        std::vector<std::shared_ptr<Texture>>   img_texture;
        std::vector<std::shared_ptr<Sprite>>    img_sprite;

        for (size_t i = 0; i < files_img.size(); i++)
        {
            std::shared_ptr<Texture> texture(new Texture);
            texture->loadFromFile(files_img[i].make_absolute().str());
            img_texture.push_back(texture);

            std::shared_ptr<Sprite> sprite (new Sprite(*texture));
            float sx = (float)Fenetre.getSize().x / (float)sprite->getTextureRect().width;
            float sy = (float)Fenetre.getSize().y / (float)sprite->getTextureRect().height;
            sprite->scale(Vector2f{ std::min(sx, sy), std::min(sx, sy) });
            img_sprite.push_back(sprite);
        }

        size_t index_img = 0;
        long cnt_loop = 0;
        do
        {
            cnt_loop++;
            if (cnt_loop > 20 * 1) // 1 sec
            {
                index_img++;
                if (index_img >= img_sprite.size() - 1)
                {
                    index_img = 0;
                }
                cnt_loop = 0;
            }

            Fenetre.clear();
            Fenetre.draw(*img_sprite[index_img]);

            Fenetre.display();

            Event event;
            while (Fenetre.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    Fenetre.close();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        } while (Fenetre.isOpen());
    }
    catch (string s)
    {
        std::cerr << s;
    }


    return 0;
}