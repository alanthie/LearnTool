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
    }

    ~VideoCapturing()
    {
        if (has_sound)
        {
            sound.stop();
        }
    }

    std::string         _file;
    cv::VideoCapture    vc;
    cv::Mat             frame;

    bool                has_sound = false;
    bool                sound_loaded = false;
    std::string         sound_file;
    sf::SoundBuffer     buffer;
    sf::Sound           sound;

    bool open()
    {
        if (_file.empty())
            return false;

        if (!vc.isOpened()) 
        {
            std::cerr << "Failed to open the video device, video file or image sequence!\n" << std::endl;
            return false;
        }

        if (has_sound)
        {
            if (buffer.loadFromFile(sound_file))
            {
                sound_loaded = true;
                sound.setBuffer(buffer);
                sound.play();
            }
        }

        return true;
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