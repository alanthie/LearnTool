//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#pragma once

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "filesystem/path.h"
#include "filesystem/resolver.h"
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <iostream>
#include <stdio.h>
#include <atomic>
#include <future>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

class VideoCapturingDeleter;

class VideoCapturing
{
public:
    VideoCapturing(const std::string& file) : _file(file), vc(file), sound_file()
    {
        filesystem::path p(file+".wav");
        if ( (p.empty() == false) && (p.exists()) && (p.is_file()) )
        {
            has_sound = true;
            sound_file = p.make_absolute().str();
        }
        else
        {
            //...
        }
    }

    void recheck_sound()
    {
        filesystem::path p(_file + ".wav");
        if ((p.empty() == false) && (p.exists()) && (p.is_file()))
        {
            has_sound = true;
            sound_file = p.make_absolute().str();
        }
    }

    ~VideoCapturing()
    {
        if (has_sound)
        {
            sound.stop();
        }

        // wait thread loading...
        if (t != nullptr)
        {
            //...in case...
            stop_thread.store(false);
            t->join();
            delete t;
            t = nullptr;
        }

        stop_thread.store(false);
    }

    std::string         _file;
    cv::VideoCapture    vc;
    cv::Mat             frame;
    std::chrono::time_point<std::chrono::system_clock> start;

    bool                has_sound = false;
    bool                sound_loaded = false;
    std::atomic<bool>   sound_isloading = false;
    std::string         sound_file;
    sf::SoundBuffer     buffer;
    sf::Sound           sound;
    std::thread*        t = nullptr;
    std::atomic<bool>   stop_thread = false;
    bool                playing_request = false;

    bool open()
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

    void load_sound()
    {
        if (has_sound)
        {
            if (sound_loaded == false)
            {
                if (sound_isloading.load() == false)
                {
                    // asych
                    sound_isloading.store(true);
                    if (t != nullptr)
                    {
                        // in case ...
                        stop_thread.store(true);
                        t->join();
                        delete t;
                        t = nullptr;
                    }
                    stop_thread.store(false);

                    // ASYNCH THREAD
                    t = new std::thread(&VideoCapturing::asych_load_sound, this);
                }
            }
        }
    }

    void asych_load_sound()
    {
        if (stop_thread.load() == false)
        {
            if (buffer.loadFromFile(sound_file)) // can not  be stop - will wait terminate in VideoCapturingDeleter thread
            {
                sound_loaded = true;
                sound.setBuffer(buffer);
            }
            else
            {
                // too fast - file not fully saved...
                // retry
                std::this_thread::sleep_for(100ms);
                if (buffer.loadFromFile(sound_file))
                {
                    sound_loaded = true;
                    sound.setBuffer(buffer);
                }
                else
                {
                    int err = 1;
                }

            }
            sound_isloading.store(false);

            play_sound();
        }
    }

    void play_sound()
    {
        if ((has_sound) && (sound_isloading.load()==false) && (sound_loaded))
        {
            if (stop_thread.load() == false)
            {
                if (playing_request == false)
                {
                    playing_request = true;
                    sound.play();
                }
            }
        }
    }

    bool readNextFrame()
    {
        vc >> frame;
        if (frame.empty())
        {
            return false;
        }
        return true;
    }

    cv::Mat& getFrame() 
    {
        return frame; 
    }
};

class VideoCapturingDeleter
{
public:
    VideoCapturingDeleter(VideoCapturing* v) : _p(v)
    {
        if (_p != nullptr)
        {
            _p->stop_thread.store(true);
            _p->sound.stop();
            t = new std::thread(&VideoCapturingDeleter::run, this);
        }
    }

    ~VideoCapturingDeleter() 
    {
        if (_p)
        {
            _p->stop_thread.store(true);
            _p->sound.stop();
            delete _p;
            _p = nullptr;
        }

        if (t)
        {
            delete t;
            t = nullptr;
        }
    }

    void run()
    {
        while (true)
        {
            if (_p != nullptr)
            {
                if (_p->t)
                {
                    if (_p->sound_isloading.load() == false)
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

    VideoCapturing* _p;
    std::thread*    t = nullptr;
    std::atomic<bool> is_done = false;
};

class ExtractSound
{
public:
    ExtractSound(const std::string& mp4_file) : _file(mp4_file)
    {
        filesystem::path p(mp4_file);
        if ((p.empty() == false) && (p.exists()) && (p.is_file()))
        {
            _thread = new std::thread(&ExtractSound::run, this);
        }
    }

    ~ExtractSound()
    {
        is_done.store(true);
        if (_thread)
        {
            _thread->join();

            delete _thread;
            _thread = nullptr;
        }
    }

    void run()
    {
        // ffmpeg -i 0001.mp4 0001.mp4.wav
        filesystem::path cmd_path("..\\tools");
        std::string cmd = cmd_path.make_absolute().str()+"\\ffmpeg.exe -i \"" + _file + "\" \"" + _file + ".wav\"";
        int r = system(cmd.c_str());

        std::cout << cmd << std::endl;

        filesystem::path wav_path(_file + ".wav");
        while (wav_path.exists() == false)
        {
            if (is_done.load() == true)
                break;
            std::this_thread::sleep_for(1ms);
        }
        is_done.store(true);
    }

    std::string     _file;
    std::thread*    _thread = nullptr;
    std::atomic<bool> is_done = false;
};