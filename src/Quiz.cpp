//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#pragma once

#include "Quiz.h"
#include "filesystem/path.h"
#include "filesystem/resolver.h"
#include "ini_parser/ini_parser.hpp"
#include <SFML/Graphics/Texture.hpp>

#include <opencv2/opencv.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/videoio.hpp>
//#include <opencv2/highgui.hpp>

#include <iostream>


int Quiz::read_xml(const std::string& filename)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.c_str());
    int  r = doc.ErrorID();

    Quiz& quiz = *this;;

    if (r == XML_SUCCESS)
    {
        {
            XMLElement* element = doc.FirstChildElement("Quiz")->FirstChildElement("Type");
            const char* str = element->GetText();
            if (strcmp(str, "one_response") == 0) quiz._type = quiz_type::one_response;
            else quiz._type = quiz_type::multi_response;
        }
        {
            XMLElement* element = doc.FirstChildElement("Quiz")->FirstChildElement("Subject");
            const char* str = element->GetText();
            quiz._subject = std::string(str);
        }
        {
            XMLElement* element = doc.FirstChildElement("Quiz")->FirstChildElement("Question");
            const char* str = element->GetText();
            quiz._question = std::string(str);
        }
        {
            XMLElement* element = doc.FirstChildElement("Quiz")->FirstChildElement("Image");
            const char* str = element->GetText();
            quiz._image = std::string(str);
        }

        {
            XMLElement* element = doc.FirstChildElement("Quiz")->FirstChildElement("Choice");
            while (true)
            {
                QuizChoice choix;
                {
                    XMLElement* element2 = element->FirstChildElement("Text");
                    const char* str = element2->GetText();
                    choix._text = std::string(str);
                }
                {
                    XMLElement* element2 = element->FirstChildElement("Response");
                    const char* str = element2->GetText();
                    choix._is_true = (std::string(str) == "true" ? true : false);
                }

                quiz._choice.push_back(choix);

                element = element->NextSiblingElement();
                if (element == nullptr)
                    break;
            }
        }
    }

    return r;
}


void QuizMaker::make_multi_image(const filesystem::path& current_path, const std::vector<filesystem::path>& img_files)
{
    std::string fsave = current_path.make_absolute().str() + "\\" + "000_all.jpg";
    filesystem::path filesave(fsave);
    if ((filesave.empty() == false) && (filesave.exists() == false))
    {
        if ((current_path.empty() == false) && (current_path.exists() == true) && (current_path.is_directory() == true))
        {
            int cnt = 0;
            std::vector<std::string> img = { "jpg",  "png", "jpeg", "bmp" };

            for (size_t i = 0; i < img_files.size(); i++)
            {
                if (std::find(img.begin(), img.end(), img_files[i].extension()) != img.end())
                {
                    cnt++;
                }
            }

            if (cnt == 0)
                return;

            int rows = 1 + (int)((cnt - 1) / 3);
            int cols = 3;
            if (cnt < 3) cols = cnt;
            const int W = 800;
            const int H = 800;

            // Create a white image
            cv::Mat3b res(rows * H, cols * W, cv::Vec3b(255, 255, 255));

            int r = 0;
            int c = -1;
            for (size_t i = 0; i < img_files.size(); i++)
            {
                if (std::find(img.begin(), img.end(), img_files[i].extension()) != img.end())
                {
                    cv::Mat3b img1 = cv::imread(img_files[i].make_absolute().str());
                    if (img1.empty() == false)
                    {
                        c++;
                        if (c >= 3)
                        {
                            r++;
                            c = 0;
                        }

                        float ratio_x = img1.cols / (float)W;
                        float ratio_y = img1.rows / (float)H;
                        float factor = factor = 1.0 / std::max(ratio_x, ratio_y);

                        cv::Mat3b outImg;
                        if (factor < 1.0)
                            cv::resize(img1, outImg, cv::Size(img1.cols * factor, img1.rows * factor), 0, 0, cv::INTER_AREA);
                        else
                            cv::resize(img1, outImg, cv::Size(img1.cols * factor, img1.rows * factor), 0, 0, cv::INTER_LINEAR);

                        // Copy image in correct position
                        outImg.copyTo(res(cv::Rect(c*W, r*H, outImg.cols, outImg.rows)));
                    }
                }
            }
            //cv::imshow("Result", res);
            cv::imwrite(fsave, res);
        }
    }
}