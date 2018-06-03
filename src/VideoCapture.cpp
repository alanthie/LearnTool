//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#pragma once

#include "VideoCapture.hpp"
#include "UIState.h"
#include <iostream>
#include <stdio.h>
#include <atomic>
#include <future>
#include <cassert>


VideoSoundCapturingDeleter::VideoSoundCapturingDeleter(VideoSoundCapturing* v) : vs_cap(v)
{
    if (vs_cap != nullptr)
    {
        vs_cap->stop_thread.store(true);
        vs_cap->music.pause();
        thread_watch_loading_sound = new std::thread(&VideoSoundCapturingDeleter::run, this);
    }
}

VideoSoundCapturingDeleter::~VideoSoundCapturingDeleter()
{
    if (vs_cap)
    {
        vs_cap->stop_thread.store(true);
        vs_cap->music.stop();

        if (thread_watch_loading_sound != nullptr)
        {
            thread_watch_loading_sound->join();
        }

        delete vs_cap;
        vs_cap = nullptr;
    }

    if (thread_watch_loading_sound != nullptr)
    {
        delete thread_watch_loading_sound;
        thread_watch_loading_sound = nullptr;
    }
}

void VideoSoundCapturingDeleter::run()
{
    while (true)
    {
        if (vs_cap != nullptr)
        {
            if (thread_watch_loading_sound != nullptr)
            {
                if (vs_cap->sound_isloading.load() == false)
                    break;
            }
            else
                break;
        }
        else
            break;
    }
    is_done.store(true);
}



VideoSoundCapturing::VideoSoundCapturing(const std::string& file, bool _auto_play) : _file(file), vc(file), sound_file(), auto_play(_auto_play)
{
    filesystem::path p(file + ".wav");
    if ((p.empty() == false) && (p.exists()) && (p.is_file()))
    {
        has_sound = true;
        sound_file = p.make_absolute().str();

        //load_sound(); // now music
    }
    else
    {
        //...
    }
}


VideoSoundCapturing* VideoSoundCapturing::find(const std::string& f, const std::vector<VideoSoundCapturing*>& vvc)
{
    VideoSoundCapturing* r = nullptr;
    for (size_t i = 0; i < vvc.size(); i++)
    {
        if (vvc[i] != nullptr)
        {
            if (vvc[i]->_file == f)
            {
                r = vvc[i];
                break;
            }
        }
    }
    return r;
}

void VideoSoundCapturing::clear(const std::string& f, std::vector<VideoSoundCapturing*>& vvc, std::vector<VideoSoundCapturingDeleter*>& v_vcd)
{
    for (size_t i = 0; i < vvc.size(); i++)
    {
        if (vvc[i] != nullptr)
        {
            if (vvc[i]->_file == f)
            {
                v_vcd.push_back(new VideoSoundCapturingDeleter(vvc[i]));
                vvc[i] = nullptr;
                break;
            }
        }
    }
}

void VideoSoundCapturing::recheck_sound()
{
    filesystem::path p(_file + ".wav");
    if ((p.empty() == false) && (p.exists()) && (p.is_file()))
    {
        if (has_sound == false)
        {
            has_sound = true;
            sound_file = p.make_absolute().str();
            
            load_sound();
        }
    }
}

VideoSoundCapturing::~VideoSoundCapturing()
{
    if (has_sound)
    {
        music.stop();
    }

    vc.release();

    // wait thread loading...
    if (thread_load_sound != nullptr)
    {
        //...in case...
        stop_thread.store(false);
        thread_load_sound->join();
        delete thread_load_sound;
        thread_load_sound = nullptr;
    }

    stop_thread.store(false);
}


bool VideoSoundCapturing::open()
{
    if (_file.empty())
        return false;

    if (!vc.isOpened())
    {
        std::cerr << "Failed to open the video device, video file or image sequence!\n" << std::endl;
        return false;
    }
    return true;
}

void VideoSoundCapturing::load_sound()
{
    if (has_sound)
    {
        if (sound_loaded == false)
        {
            if (sound_isloading.load() == false)
            {
                // asych
                sound_isloading.store(true);
                if (thread_load_sound != nullptr)
                {
                    // in case ...
                    stop_thread.store(true);
                    thread_load_sound->join();
                    delete thread_load_sound;
                    thread_load_sound = nullptr;
                }
                stop_thread.store(false);

                // ASYNCH THREAD
                thread_load_sound = new std::thread(&VideoSoundCapturing::asych_load_sound, this);
            }
        }
    }
}

void VideoSoundCapturing::asych_load_sound()
{
    if (stop_thread.load() == false)
    {
        int n = 0;
        while (music.openFromFile(sound_file) == false) // TODO file being build...
        {
            n++;
            std::this_thread::sleep_for(500ms);
            if (n > 60)
            {
                // quit
                sound_loaded = false;
                sound_isloading.store(false);
                return;
            }
        }

        sound_loaded = true;
        sound_isloading.store(false);

        if (auto_play)
        {
            play_sound();
        }
    }
}

void VideoSoundCapturing::play_sound()
{
    if ((has_sound) && (sound_isloading.load() == false) && (sound_loaded))
    {
        if (stop_thread.load() == false)
        {
            if (playing_request == false)
            {
                playing_request = true;
                music.play();
            }
        }
    }
}

bool VideoSoundCapturing::readNextFrame()
{
    vc >> frame;
    if (frame.empty())
    {
        return false;
    }
    return true;
}

cv::Mat& VideoSoundCapturing::getFrame()
{
    return frame;
}