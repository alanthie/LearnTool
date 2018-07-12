//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#pragma once

#include "Quiz.h"


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