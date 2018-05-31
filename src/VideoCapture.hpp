//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier - All Rights Reserved  
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
    VideoCapturing(const std::string& file) : _file(file), vc(file)
    {
    }

    std::string         _file;
    cv::VideoCapture    vc;
    cv::Mat             frame;

    bool open()
    {
        if (_file.empty())
            return false;

        //if (!vc.isOpened())     // if this fails, try to open as a video camera, through the use of an integer param
        //    vc.open(atoi(_file.c_str()));

        if (!vc.isOpened()) 
        {
            std::cerr << "Failed to open the video device, video file or image sequence!\n" << std::endl;
            return false;
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