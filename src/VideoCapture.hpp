//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#pragma once

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <atomic>
#include <future>

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
            // if option... make/save *.wav file in background - launch system command
            //...
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

    bool                has_sound = false;
    bool                sound_loaded = false;
    std::atomic<bool>   sound_isloading = false;
    std::string         sound_file;
    sf::SoundBuffer     buffer;
    sf::Sound           sound;
    std::thread*        t = nullptr;
    std::atomic<bool>   stop_thread = false;
    int                 pass_n = 0;

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
            sound_isloading.store(false);

            play_sound();
        }
    }

    void play_sound()
    {
        if ((has_sound)&& (sound_isloading.load()==false))
        {
            if (stop_thread.load() == false)
            {
                sound.play();
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