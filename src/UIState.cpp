//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#include "UIState.h"
#include "SFML_SDK/Game.h"
#include "SFML_SDK/GUI/Button.h"
#include "SFML_SDK/GUI/Textbox.h"
#include "SFML_SDK/States/StateBase.h"
#include "SFML_SDK/GUI/StackMenu.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui.hpp"

#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cassert>

void UIState::img_changed()
{
    minimap.reset();
    canvas_scale = { 1.0f, 1.0f };
    cnt_loop = 0;

    if (_vc != nullptr)
    {
        _vc->music.pause();
        
        // keep in v_vc cache
        _vc = nullptr;
    }

    //  cleanup v_vcd
    if (v_vcd.size() > 0)
    {
        bool all_done = true;
        for( size_t i=0; i< v_vcd.size(); i++)
        {
            VideoSoundCapturingDeleter* item_deleter = v_vcd[i];
            if (item_deleter != nullptr)
            {
                if (item_deleter->vs_cap != nullptr)
                {
                    //if (item_deleter->is_done.load() == false)
                    //{
                    //    all_done = false;
                    //    break;
                    //}
                }
                else
                {
                    delete item_deleter;
                    v_vcd[i] = nullptr;
                }
            }
        }

        if (all_done)
        {
            v_vcd.clear();
        }
    }

    // v_vc
    if (v_vc.size() > 0)
    {
        for (size_t i = 0; i< v_vc.size(); i++)
        {
            int n = 0; 
            for (size_t j = i+1; j < v_vc.size(); j++)
            { 
                if (v_vc[j] != nullptr)
                {
                    n++;
                }
            }

            VideoSoundCapturing* item_vc = v_vc[i];
            if (item_vc != nullptr)
            {
                //...locate middle...
                if (n > 10) // only 256 vc music allowed...
                {
                    VideoSoundCapturing::clear(item_vc->_file, v_vc, v_vcd);
                }
            }
            else
            {
            }
        }
    }
}

void UIState::widget_changed(std::string& b_name)
{
    // minimap changed...
}

void UIState::widget_clicked(std::string& b_name)
{
    if (b_name == "b_pause")
    {
        is_pause = !is_pause;
        if (is_pause)
        {
			button_menu[0][0]->setText("continue");
            if (_vc != nullptr)
            {
                if (_vc->has_sound)
                {
                    _vc->music.pause();
                }
            }
        }
        else
        {
            button_menu[0][0]->setText("pause");
            if (_vc != nullptr)
            {
                _vc->pause_unpause_pending = true;
                if (_vc->has_sound)
                {
                    _vc->music.play();
                }
            }
        }
    }

    else if (b_name == "b_shot")
    {	
        if ((_mode == display_mode::show_movie) && (_vc != nullptr))
        {
            cv::Mat frameRGBA;
            cv::Mat frameRGB = _vc->getFrame();
            if (!frameRGB.empty())
            {
                cv::cvtColor(frameRGB, frameRGBA, cv::COLOR_BGR2RGBA);
                {
                    std::vector<int> compression_params;
                    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
                    compression_params.push_back(100);

                    {
                        long np = (long)_vc->vc.get(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES);
                        std::string filePath = _fnav.current_path.make_absolute().str()  + "\\" + img_files[index_img].filename()
                                                + "_" + to_string(static_cast<long long>(np)) + ".jpg";                      
                        cv::imwrite(filePath, frameRGB, compression_params);
                    }
                }
            }
        }
    }

    else if (b_name == "b_img_next")
    {
        if (index_img == img_files.size() - 1)
        {
            _fnav.next_path();
            img_changed();
        }
        else
        {
            index_img++;
            if (index_img > img_files.size() - 1)
            {
                index_img = 0;
            }
            img_changed();
        }
    }
    else if (b_name == "b_img_prev")
    {
        if (index_img == 0)
        {
            _fnav.prev_path();
            img_changed();
        }
        else
        {
            index_img--;
            if (index_img < 0)
            {
                index_img = (long)img_files.size() - 1;
            }
            img_changed();
        }
    }

    else if (b_name == "b_topic_prev")
    {
        _fnav.prev_path();
        img_changed();
    }
    else if (b_name == "b_topic_next")
    {
        _fnav.next_path();
        img_changed();
    }

    else if (b_name == "b_zoom_plus")
    {
        canvas_scale = canvas_scale * ui.cfg.zoom;
        minimap.set_view(canvas_w, canvas_h, canvas_bounds);
    }
    else if (b_name == "b_zoom_less")
    {
        canvas_scale = canvas_scale / ui.cfg.zoom;
        minimap.set_view(canvas_w, canvas_h, canvas_bounds);
    }

	else if (b_name == "b_speed_slow")
	{
        if (_mode == display_mode::show_img)
        {
            vitesse_img_sec += 1.0f;
        }
        else
        {
            vitesse_video_factor /= 1.25f;
        }
        if (_vc != nullptr)
            _vc->speed_changed_pending = true;
	}
	else if (b_name == "b_speed_fast")
	{
        if (_mode == display_mode::show_img)
        {
            vitesse_img_sec -= 1.0f;
            if (vitesse_img_sec <= 0.25)
                vitesse_img_sec = 0.25f;
        }
        else
        {
            vitesse_video_factor *= 1.25f;
        }
        if (_vc != nullptr)
            _vc->speed_changed_pending = true;
	}

    else if (b_name == "b_vol_plus")
    {
        sound_volume *= 1.10f; 
        if (sound_volume > 100.0) sound_volume = 100.0f;
        if (_vc != nullptr)
        {
            _vc->music.setVolume(sound_volume);
        }
    }
    else if (b_name == "b_vol_less")
    {
        sound_volume /= 1.10f;
        if (sound_volume < 0.0) sound_volume = 0.0f;
        if (_vc != nullptr)
        {
            _vc->music.setVolume(sound_volume);
        }
    }
}


UIState::UIState(UImain& g) : 
	StateBase(g),
	ui(g),
    _fnav(*this, ui.cfg.path_dir, ui.cfg.exclude_folder, ui.cfg.img),
	button_name(    "b_name",   gui::ButtonSize::Small),
	button_parts(   "b_parts",  gui::ButtonSize::Wide),
	button_msg(     "b_msg",    gui::ButtonSize::Wide),
	minimap(        "mmap",     50, 50)
{
	button_name.m_text.setFont( ResourceHolder::get().fonts.get("arial"));
	button_parts.m_text.setFont(ResourceHolder::get().fonts.get("arial"));
	button_msg.m_text.setFont(  ResourceHolder::get().fonts.get("arial"));

	button_msg.m_text.setOrigin(0.0f, 0.0f);
	button_menu[0][0] = new gui::Button("b_pause", gui::ButtonSize::Small);
	button_menu[1][0] = new gui::Button("b_img_prev", gui::ButtonSize::Small);
	button_menu[1][1] = new gui::Button("b_img_next", gui::ButtonSize::Small);
	button_menu[2][0] = new gui::Button("b_zoom_plus", gui::ButtonSize::Small);
	button_menu[2][1] = new gui::Button("b_zoom_less", gui::ButtonSize::Small);
	button_menu[3][0] = new gui::Button("b_topic_prev", gui::ButtonSize::Small);
	button_menu[3][1] = new gui::Button("b_topic_next", gui::ButtonSize::Small);
    button_menu[4][0] = new gui::Button("b_shot", gui::ButtonSize::Small);
    button_menu[4][1] = nullptr;
	button_menu[5][0] = new gui::Button("b_speed_slow", gui::ButtonSize::Small);
	button_menu[5][1] = new gui::Button("b_speed_fast", gui::ButtonSize::Small);
    button_menu[6][0] = new gui::Button("b_vol_plus", gui::ButtonSize::Small);
    button_menu[6][1] = new gui::Button("b_vol_less", gui::ButtonSize::Small);

    float b_w = button_menu[0][0]->m_text.getLocalBounds().width;
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (button_menu[i][j] != nullptr)
            {
                button_menu[i][j]->m_rect.setFillColor(sf::Color::Black);
                button_menu[i][j]->m_rect.setOutlineColor(sf::Color::Black);

                button_menu[i][j]->setFunction(&StateBase::widget_clicked);
                button_menu[i][j]->m_rect.setSize({ b_w , b_h });
            }
        }
    }

	button_menu[0][0]->setText("pause");
	button_menu[1][0]->setText("<");
	button_menu[1][1]->setText(">");
	button_menu[2][0]->setText("+");
	button_menu[2][1]->setText("-");
	button_menu[3][0]->setText("<<");
	button_menu[3][1]->setText(">>");
    button_menu[4][0]->setText("shot");
    //button_menu[4][1
	button_menu[5][0]->setText("slower");
	button_menu[5][1]->setText("faster");
    button_menu[6][0]->setText("vol +");
    button_menu[6][1]->setText("vol -");
    
    button_menu[0][0]->m_rect.setSize({ 2 * b_w , b_h });
    button_menu[4][0]->m_rect.setSize({ 2 * b_w , b_h });

    minimap.m_rect.setSize({ 2 * b_w , 2 * b_w, });

    button_name.setFunction(    &StateBase::widget_clicked);
    button_parts.setFunction(   &StateBase::widget_clicked);
    button_msg.setFunction(     &StateBase::widget_clicked);

    minimap.setFunction(&StateBase::widget_changed);

    if (_fnav.current_path.empty() == false)
    {
        load_path(_fnav.current_path);
    }

    std::cout <<"Using OpenCV version " << CV_VERSION << "\n" << std::endl;
    std::cout << cv::getBuildInformation();
}


void UIState::handleEvent(sf::Event e) 
{
    switch (e.type)
    {
    case sf::Event::Closed:
        break;

    case sf::Event::Resized:
        //m_pGame->getWindow().setView(sf::View(sf::FloatRect(0, 0, (float)e.size.width, (float)e.size.height)));
        recalc_size();

        break;

    default:
        break;
    }

    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (button_menu[i][j] != nullptr)
            {
                button_menu[i][j]->handleEvent(e, m_pGame->getWindow(), *this);
            }
        }
    }

    button_name.handleEvent(e,  m_pGame->getWindow(), *this);
    button_parts.handleEvent(e, m_pGame->getWindow(), *this);
    button_msg.handleEvent(e,   m_pGame->getWindow(), *this);
    minimap.handleEvent(e,      m_pGame->getWindow(), *this);
}

void UIState::handleInput() 
{
}

void UIState::update(sf::Time deltaTime) 
{
    if (_mode == display_mode::show_img)
    {
        if (is_pause == false)
        {
            cnt_loop++;
            if (cnt_loop > 60 * vitesse_img_sec) // 1 sec * vitesse_img_sec
            {
                index_img++;
                if (index_img > img_files.size() - 1)
                {
                    _fnav.next_path();
                }
                img_changed();
            }
        }
    }

    if (_mode == display_mode::show_movie)
    {
        if (is_pause == false)
        {
            if (cnt_loop > 0)   // 0 means first time
            {
                if (_vc == nullptr)
                {
                    index_img++;
                    if (index_img > img_files.size() - 1)
                    {
                        _fnav.next_path();
                    }
                    img_changed();
                }
            }
            cnt_loop++;
        }
    }

    //---------------------------------
    // sound making quota
    //---------------------------------
    if (ui.cfg.mak_wav_file == 1)
    {
        if (v_extract_sound.size() > 0)
        {
            int n_running = 0;
            for (const auto item : v_extract_sound)
            {
                if (item != nullptr)
                {
                    if ((item->is_done.load() == false) && (item->is_started.load() == true))
                    {
                        n_running++;
                    }
                }
            }

            if ((n_running < ui.cfg.make_N_sound_file) && (v_extract_sound.size() > 0))
            {
                for (const auto item : v_extract_sound)
                {
                    if (item != nullptr)
                    {
                        if ((item->is_done.load() == false) && (item->is_started.load() == false))
                        {
                            item->is_started.store(true);
                            break;
                        }
                    }
                }
            }

            // cleanup
            bool all_done = true;
            for (const auto item : v_extract_sound)
            {
                if (item != nullptr)
                {
                    if (item->is_done.load() == false)
                    {
                        all_done = false;
                        break;
                    }
                }
            }

            if (all_done)
            {
                v_extract_sound.clear();
            }
        }
    }


    //---------------------------------
    // sound loading 
    //---------------------------------
    if (ui.cfg.load_sound_file == 1)
    {
        for (size_t i = 0; i < v_vc.size(); i++)
        {
            if (v_vc[i] != nullptr)
            {
                if (v_vc[i]->has_sound)
                {
                    if (v_vc[i]->sound_loaded == false)
                    {
                        v_vc[i]->load_sound();
                    }
                }
            }
        }
    }

}

void UIState::fixedUpdate(sf::Time deltaTime) 
{

}

void UIState::render(sf::RenderTarget& renderer) 
{
    button_msg.setText("");

    recalc_size();
    if (img_files.size() > 0)
    {
        std::string s = img_files[index_img].extension();
        if ((s == "mp4") || (s == "avi"))
        {
            _mode = display_mode::show_movie;
        }
        else
        {
            _mode = display_mode::show_img;
        }
    }

    // main_view
    m_pGame->getWindow().setView(main_view);
    minimap.render(renderer);

    if (_mode == display_mode::show_img)
    {
        if (img_files.size() > 0)
        {
            assert(index_img >= 0);
            assert(index_img <= img_files.size() - 1);

            if (img_texture[index_img].get() == nullptr)
            {
                img_texture[index_img] = std::shared_ptr<sf::Texture>(new sf::Texture);
                img_texture[index_img]->loadFromFile(img_files[index_img].make_absolute().str());
            }

            if (img_texture[index_img].get() != nullptr)
            {
                if (index_img < img_files.size())
                {
                    button_msg.setText( "[" + std::to_string(1 + (long)index_img) + "/" + std::to_string((long)img_files.size()) + "] " +
                                         img_files[index_img].filename() +
                                         "[" + std::to_string(vitesse_img_sec) + "," + std::to_string(vitesse_video_factor) + "]" +
                                         "[W" + std::to_string(count_sound_making()) + "]");
                }

                sprite_canva.reset();
                sprite_canva = std::shared_ptr<sf::Sprite>(new sf::Sprite(*img_texture[index_img].get()));
                sprite_canva->scale(scale_sprite(sprite_canva));
                sprite_canva->scale(canvas_scale);
                sprite_canva->move(-1.0f * minimap.ratio_offset.x * canvas_w, -1.0f * minimap.ratio_offset.y * canvas_h);
                canvas_bounds = sprite_canva->getGlobalBounds();
                renderer.draw(*(sprite_canva.get()));
            }
        }
 
        // view_minimap
        if (img_texture.size() > 0)
        {
            if (img_texture[index_img].get() != nullptr)
            {
                sprite_canva.reset();
                sprite_canva = std::shared_ptr<sf::Sprite>(new sf::Sprite(*img_texture[index_img].get()));

                sf::FloatRect acanvas_bounds = sprite_canva->getLocalBounds();
                view_minimap.setCenter((acanvas_bounds.width) / 2.0f, (acanvas_bounds.height) / 2.0f);
                view_minimap.setSize(acanvas_bounds.width, acanvas_bounds.height);

                m_pGame->getWindow().setView(view_minimap);
                renderer.draw(*(sprite_canva.get()));
                m_pGame->getWindow().setView(main_view);
            }
        }
    }

    // main_view
    bool done = false;
    bool new_entry = false;
    if (_mode == display_mode::show_movie)
    {
        //if (is_pause == false)
        {
            if (_vc == nullptr)
            {
                if (img_files.size() > 0)
                {
                    if (img_files[index_img].empty() == false)
                    {
                        std::string msg;
                        if (index_img < img_files.size())
                        {
                            msg = "[" + std::to_string(1 + (long)index_img) + "/" + std::to_string(0 + (long)img_files.size()) + "] " +
                                img_files[index_img].filename() +
                                "[" + std::to_string(vitesse_img_sec) + "," + std::to_string(vitesse_video_factor) + "]"
                                + "[W" + std::to_string(count_sound_making()) + "]";
                            button_msg.setText(msg);
                        }

                        VideoSoundCapturing* r = VideoSoundCapturing::find(img_files[index_img].make_absolute().str(), v_vc);
                        if (r != nullptr)
                        {
                            //-------------------------------------
                            // VideoSoundCapturing already in cache
                            //-------------------------------------
                            _vc = r;
                            if (is_pause == false)
                                _vc->play_sound();
                            _vc->music.setVolume(sound_volume);
                        }
                        else
                        {
                            //----------------------------------
                            // new VideoSoundCapturing
                            // ----------------------------------
                            _vc = new VideoSoundCapturing(img_files[index_img].make_absolute().str());
                            _vc->music.setVolume(sound_volume);
                            v_vc.push_back(_vc);
                        }
                        
                        new_entry = true;
                        long np = (long)_vc->vc.get(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES);
                        _vc->entry_frame = np;

                        if (_vc->open() == false)
                        {
                            _vc->music.pause();
                            VideoSoundCapturing::clear(_vc->_file, v_vc, v_vcd);
                            _vc = nullptr;
                        }
                        else
                        {
                            if (_vc->has_sound)
                            {
                                if (_vc->sound_loaded == false)
                                {
                                    _vc->load_sound();
                                }
                            }
                            else
                            {   
                                // Create sound file 
                                if (ui.cfg.mak_wav_file == 1)
                                {
                                    v_extract_sound.push_back(new ExtractSound(img_files[index_img].make_absolute().str()));
                                }
                            }
                        }
                    }
                }
            }

            if (_vc != nullptr)
            {
                if (ui.cfg.load_sound_file == 1)
                {
                    if (ui.cfg.mak_wav_file == 1)
                    {
                        if (_vc->sound_loaded == false)
                        {
                            _vc->recheck_sound();
                        }
                    }

                    if (_vc->has_sound == true)
                    {
                        if (_vc->music.getStatus() != sf::SoundSource::Status::Playing)
                        {
                            if (_vc->sound_loaded == false)
                            {
                                _vc->load_sound();                     
                            }

                            if (_vc->sound_loaded == true)
                            {
                                if (is_pause == false)
                                    _vc->play_sound();

                                std::string msg;
                                msg =   "[" + std::to_string(1 + (long)index_img) + "/" + std::to_string(0 + (long)img_files.size()) + "] " +
                                        img_files[index_img].filename() +
                                        "[" + std::to_string(vitesse_img_sec) + "," + std::to_string(vitesse_video_factor) + "]" +
                                        "[W" + std::to_string(count_sound_making()) + "]";

                                button_msg.setText(msg);
                            }
                        }
                    }
                }

                if (is_pause == false)
                {
                    long np = (long)_vc->vc.get(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES);
                    long nc = (long)_vc->vc.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT);
                    double fps = _vc->vc.get(cv::VideoCaptureProperties::CAP_PROP_FPS);

                    if ( (new_entry == true) && (_vc->done == true) )
                    {
                        // reset frame
                        np = 0;
                        _vc->entry_frame = 0;
                        _vc->vc.set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, 0);
                        _vc->done = false;
                        if (is_pause == false)
                            _vc->play_sound();
                        _vc->music.setPlayingOffset(sf::seconds((float)0)); // if fps frame/sec
                    }
                    else if (_vc->pause_unpause_pending == true)
                    {
                       _vc->entry_frame = np;
                       _vc->vc.set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, np);
                       _vc->pause_unpause_pending = false;
                       if (is_pause == false)
                         _vc->play_sound();
                       float frame_time = (float)(np / fps);
                       _vc->music.setPlayingOffset(sf::seconds(frame_time));
                    }
                    else if (_vc->speed_changed_pending == true)
                    {
                        _vc->entry_frame = np;
                        _vc->vc.set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, np);
                        _vc->speed_changed_pending = false;
                        float frame_time = (float)(np / fps);
                        if (is_pause == false)
                            _vc->play_sound();
                        _vc->music.setPlayingOffset(sf::seconds(frame_time));
                    }
                    if (new_entry == true)
                    {
                        _vc->entry_frame = np;
                        _vc->vc.set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, np);
                        _vc->speed_changed_pending = false;
                        float frame_time = (float)(np / fps);
                        if (is_pause == false)
                            _vc->play_sound();
                        _vc->music.setPlayingOffset(sf::seconds(frame_time));
                    }

                    fps = fps * vitesse_video_factor;

                    bool skip = false;
                    int pass_n = 0;
                    if (np == _vc->entry_frame)
                    {
                        _vc->start = std::chrono::system_clock::now();
                        _vc->music.setVolume(sound_volume);
                    }
                    else if (std::abs(vitesse_video_factor - 1.0f) > 0.01)
                    {
                        auto end = std::chrono::system_clock::now();
                        std::chrono::duration<double> diff_sec = end - _vc->start;

                        // ???

                        long target = _vc->entry_frame + (long) ( fps * diff_sec.count() );
                        if (target > np + 1)
                        {
                            // read more
                            pass_n = target -( np + 1);
                        }
                        else if (target < np - 1)
                        {
                            // read less
                            skip = true;
                        }
                    }                   
                    else if ( (std::abs(vitesse_video_factor - 1.0f) <= 0.01) && (_vc->has_sound == true) )
                    {
                        // TODO: Reset sound or frame if sound was just created...

                        sf::Time t = _vc->music.getPlayingOffset();
                        float tsec = t.asSeconds();
                        float frame_time = (float)(np / fps);
                        if ((tsec > 0.00) && (tsec < frame_time - (1.0f/fps)))
                        {
                            // WAIT SOUND!
                            skip = true;
                            pass_n = 0;
                        }
                    }

                    while (pass_n > 0)
                    {
                        pass_n--;
                        if (_vc->readNextFrame() == false)
                        {
                            done = true;
                            _vc->done = true;
                            break;
                        }
                    }

                    if (skip == false)
                    {
                        if (_vc->readNextFrame() == false)
                        {
                            done = true;
                            _vc->done = true;
                        }
                    }
                }
                else
                {
                    long np = (long)_vc->vc.get(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES);
                    if ((np == _vc->entry_frame) && (np == 0))
                    {
                        // show 1th frame
                        if (_vc->readNextFrame() == false)
                        {
                            done = true;
                            _vc->done = true;
                        }
                    }
                }

                if (done == false)
                {
                    cv::Mat frameRGBA;
                    sf::Image image;
                    sf::Texture texture;

                    cv::Mat frameRGB = _vc->getFrame();
                    if (!frameRGB.empty())
                    {
                        cv::cvtColor(frameRGB, frameRGBA, cv::COLOR_BGR2RGBA);
                        image.create(frameRGBA.cols, frameRGBA.rows, frameRGBA.ptr());
                        if (texture.loadFromImage(image))
                        {
                            sprite_canva.reset();
                            sprite_canva = std::shared_ptr<sf::Sprite>(new sf::Sprite(texture));
                            sprite_canva->scale(scale_sprite(sprite_canva));
                            sprite_canva->scale(canvas_scale);
                            sprite_canva->move(-1.0f * minimap.ratio_offset.x * canvas_w, -1.0f * minimap.ratio_offset.y * canvas_h);
                            canvas_bounds = sprite_canva->getGlobalBounds();
                            renderer.draw(*(sprite_canva.get()));
     
                            // view_minimap
                            {
                                sprite_canva.reset();
                                sprite_canva = std::shared_ptr<sf::Sprite>(new sf::Sprite(texture));

                                sf::FloatRect acanvas_bounds = sprite_canva->getLocalBounds();
                                view_minimap.setCenter((acanvas_bounds.width) / 2.0f, (acanvas_bounds.height) / 2.0f);
                                view_minimap.setSize(acanvas_bounds.width, acanvas_bounds.height);

                                m_pGame->getWindow().setView(view_minimap);
                                renderer.draw(*(sprite_canva.get()));
                                m_pGame->getWindow().setView(main_view);
                            }
                        }

                        double np = _vc->vc.get(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES); // retrieves the current frame number
                        double nc = _vc->vc.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT);
                        button_msg.setText("[" + std::to_string(1 + (long)index_img) + "/" + std::to_string(0 + (long)img_files.size()) + "] " +
                            img_files[index_img].filename() + " - " + std::to_string((long)np) + "/" + std::to_string((long)nc)
                            + "[" + std::to_string(vitesse_img_sec) + "," + std::to_string(vitesse_video_factor) + "]"
                            + "[W" + std::to_string(count_sound_making()) + "]");
                    }
                }
                else
                {
                    // done
                    _vc->music.pause();

                    // keep for a awhile
                    _vc = nullptr;
                }
            }
        }
    }

    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (button_menu[i][j] != nullptr)
            {
                if (button_menu[i][j]->hasMouse(ui.getWindow()))
                {
                    button_menu[i][j]->m_rect.setFillColor(sf::Color::Green);
                    button_menu[i][j]->m_rect.setOutlineColor(sf::Color::Black);
                }
                else
                {
                    button_menu[i][j]->m_rect.setFillColor(sf::Color::Black);
                    button_menu[i][j]->m_rect.setOutlineColor(sf::Color::Green);
                }

                button_menu[i][j]->render(renderer);
            }
        }
    }

    button_name.render(renderer);
    button_parts.render(renderer);
    button_msg.render(renderer);

    renderer.draw(minimap.m_drag_rect);
}

void UIState::recalc_size()
{
    w = (float)ui.getWindow().getSize().x;
    h = (float)ui.getWindow().getSize().y;

    canvas_w = (float)(canvas_x_perc * w);
    canvas_h = (float)(h - 2 * b_h - 1.0f);

    float b_w = (float)(w - canvas_w - 1) / 2;

    button_menu[0][0]->setPosition({ canvas_w, 1 });
    button_menu[0][0]->m_rect.setSize({ 2 * b_w  , b_h });
    button_menu[1][0]->m_rect.setSize({ b_w , b_h });
    button_menu[1][1]->m_rect.setSize({ b_w  , b_h });
    button_menu[2][0]->m_rect.setSize({ b_w , b_h });
    button_menu[2][1]->m_rect.setSize({ b_w , b_h });
    button_menu[3][0]->m_rect.setSize({ b_w , b_h });
    button_menu[3][1]->m_rect.setSize({ b_w , b_h });
	button_menu[4][0]->m_rect.setSize({ 2 * b_w , b_h });
	//button_menu[4][1]
    button_menu[5][0]->m_rect.setSize({ b_w , b_h });
    button_menu[5][1]->m_rect.setSize({ b_w , b_h });
    button_menu[6][0]->m_rect.setSize({ b_w , b_h });
    button_menu[6][1]->m_rect.setSize({ b_w , b_h });

    button_menu[1][0]->setPosition({ canvas_w, b_h });
    button_menu[1][1]->setPosition({ canvas_w + b_w, b_h });
    button_menu[2][0]->setPosition({ canvas_w, 2*b_h });
    button_menu[2][1]->setPosition({ canvas_w + b_w, 2*b_h });
    button_menu[3][0]->setPosition({ canvas_w, 3 * b_h });
    button_menu[3][1]->setPosition({ canvas_w + b_w, 3 * b_h });
    button_menu[4][0]->setPosition({ canvas_w, 8 * b_h });
    // button_menu[4][1]
	button_menu[5][0]->setPosition({ canvas_w, 9 * b_h });
	button_menu[5][1]->setPosition({ canvas_w + b_w, 9 * b_h });
    button_menu[6][0]->setPosition({ canvas_w, 10 * b_h });
    button_menu[6][1]->setPosition({ canvas_w + b_w, 10 * b_h });

    float mmap_w = 2 * b_w;
    minimap.m_rect.setSize({ mmap_w , 4 * b_h, });
    if (minimap.moving == false)
    {
        minimap.setPosition({ canvas_w, 4 * b_h });
    }

    button_name.setPosition({ (float)1, canvas_h });
    button_name.m_rect.setSize({ (float)((button_parts.m_text.getString().getSize() == 0)? w-1: w/3 ) , b_h });

    button_parts.setPosition({ button_name.getSize().x , canvas_h });
    button_parts.m_rect.setSize({ w - (button_name.getSize().x + 1) , b_h });

    button_msg.m_rect.setSize({ w - 2.0f, b_h });
    button_msg.setPosition({ 1 , canvas_h + b_h });

    main_view.setCenter(w / 2.0f, h / 2.0f);
    main_view.setSize(w, h);
    main_view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));

    view_minimap.setCenter(canvas_w / 2.0f, canvas_h / 2.0f);
    view_minimap.setSize(canvas_w, canvas_h);
    view_minimap.setViewport(sf::FloatRect(minimap.m_rect.getPosition().x / w , minimap.m_rect.getPosition().y / h, minimap.m_rect.getSize().x / w, minimap.m_rect.getSize().y / h));
}

sf::Vector2f UIState::scale_sprite(std::shared_ptr<sf::Sprite> sprite)
{
    float sx = (canvas_w) / (float)sprite->getTextureRect().width;
    float sy = (canvas_h) / (float)sprite->getTextureRect().height;
    return sf::Vector2f{ std::min(sx, sy), std::min(sx, sy) };
}

void UIState::load_path(filesystem::path& p)
{
    ini_filename.clear();
    img_files.clear();
    img_texture.clear();

    index_img = 0;
    cnt_loop = 0;
    ini.reset();

    button_name.setText("");
    button_parts.setText("");

    std::string fullname = p.make_absolute().str();
    std::string name = fullname.substr(fullname.find(_fnav.root.make_absolute().str()) + _fnav.root.make_absolute().str().size());
    std::string desc;

    std::cout << "Name:" << name << std::endl;

    ui.getWindow().setTitle(ui.cfg.title + " [" + p.make_absolute().str() + "]");

    std::vector<std::string> files = filesystem::path::get_directory_file(p, false);
    for (size_t i = 0; i < files.size(); i++)
    {
        filesystem::path pv = files.at(i);
        if (pv.is_file())
        {
            std::string s = pv.extension();
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            if (std::find(ui.cfg.img.begin(), ui.cfg.img.end(), s) != ui.cfg.img.end())
            {
                img_files.push_back(pv);
            }

            if (pv.extension() == "ini")
            {
                if (pv.filename() == "desc.ini")
                {
                    ini = std::shared_ptr<ini_parser>(new ini_parser(pv.make_absolute().str()));

                    try
                    {
                        name = ini->get_string("name", "main");
                    }
                    catch (...)
                    {
                    }

                    std::string key;
                    try
                    {
                        key = ini->get_string("key", "parts");
                    }
                    catch (...)
                    {
                    }

                    std::string value;
                    try
                    {
                        value = ini->get_string("value", "parts");
                    }
                    catch (...)
                    {
                    }

                    if (key.size() > 0)
                    {
                        std::vector<std::string> parts = Config::split(value, ';');
                        desc = key + ": " + Config::merge(parts);
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < img_files.size(); i++)
    {
        std::shared_ptr<sf::Texture> texture(nullptr);
        img_texture.push_back(texture);
    }

    button_name.setText(name);
    button_parts.setText(desc);
    button_msg.setText("");
}

int UIState::count_sound_making()
{
    int n = 0;
    for (size_t i = 0; i < v_extract_sound.size(); i++)
    {
        if (v_extract_sound[i] != nullptr)
        {
            if ((v_extract_sound[i]->is_done.load() == false) && (v_extract_sound[i]->is_started.load() == true))
            {
                n++;
            }
        }
    }
    return n;
}