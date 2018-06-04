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
#include <SFML/Audio.hpp>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <atomic>
#include <future>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

class VideoSoundCapturing;

class VideoSoundCapturingDeleter
{
public:
    VideoSoundCapturingDeleter(VideoSoundCapturing* v);
    ~VideoSoundCapturingDeleter();

    void run();

    VideoSoundCapturing*    vs_cap;
    std::thread*            thread_watch_loading_sound = nullptr;
    std::atomic<bool>       is_done = false;
};


class VideoSoundCapturing
{
public:
    VideoSoundCapturing(const std::string& file, bool _auto_play = false);
    ~VideoSoundCapturing();

    static VideoSoundCapturing*  find( const std::string& f, const std::vector<VideoSoundCapturing*>& vvc);
    static void                  clear(const std::string& f, std::vector<VideoSoundCapturing*>& vvc, std::vector<VideoSoundCapturingDeleter*>& v_vcd);
    
    std::string         _file;
    cv::VideoCapture    vc;
    cv::Mat             frame;
    long                entry_frame = 0;
    std::chrono::time_point<std::chrono::system_clock> start;
    bool                done = false;
    bool                pause_unpause_pending = false;

    bool                has_sound = false;
    bool                sound_loaded = false;
    std::atomic<bool>   sound_isloading = false;
    std::string         sound_file;
    sf::Music           music;
    std::thread*        thread_load_sound = nullptr;
    std::atomic<bool>   stop_thread = false;
    bool                playing_request = false;
    bool                auto_play = false;

    bool open();
    void load_sound();
    void asych_load_sound();
    void play_sound();
    void recheck_sound();
    bool readNextFrame();
    cv::Mat& getFrame();
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
        while (is_started.load() == false)
        {
            std::this_thread::sleep_for(1000ms);
        }

        // ffmpeg -i 0001.mp4 0001.mp4.wav
        filesystem::path cmd_path("..\\tools");
        std::string cmd = cmd_path.make_absolute().str()+"\\ffmpeg.exe -i \"" + _file + "\" \"" + _file + ".wav\"";
        std::cout << cmd << std::endl;
        int r = system(cmd.c_str());

        filesystem::path wav_path(_file + ".wav");
        while (wav_path.exists() == false)
        {
            // check size...

            if (is_done.load() == true)
                break;
            std::this_thread::sleep_for(1ms);
        }
        is_done.store(true);
    }

    std::atomic<bool> is_started = false;
    std::string     _file;
    std::thread*    _thread = nullptr;
    std::atomic<bool> is_done = false;
};
