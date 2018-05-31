//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#pragma once
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>
#include "filesystem/path.h"
#include "filesystem/resolver.h"
#include "ini_parser/ini_parser.hpp"

class Config
{
public:
    std::string     title = "Learning Tool";
    int             default_w = 900;
    int             default_h = 600;
    std::string     path_dir = ".\\";
    float           zoom = 1.25;

    std::vector<std::string> exclude_folder = { ".Thumbs" };
    std::vector<std::string> img = { "jpg",  "png", "gif", "jpeg", "bmp", "mp4" , "avi" };

    Config() = default;

	bool setup(std::string config_file)
	{
		if (config_file.size() > 0)
		{
		    filesystem::path p(config_file);
		    if ((p.empty() == false) && (p.exists() == true) && (p.is_file() == true))
		    {
		        std::shared_ptr<ini_parser> cfg_ini = std::shared_ptr<ini_parser>(new ini_parser(p.make_absolute().str()));
		        if (cfg_ini)
		        {
		            std::string path_dir_temp;
		            try
		            {
		                path_dir_temp = cfg_ini->get_string("path_folder", "main");
		                filesystem::path path_folder(path_dir_temp);
		                if ((path_folder.empty() == false) && (path_folder.exists() == true) && (path_folder.is_directory() == true))
		                {
		                    this->path_dir = path_folder.make_absolute().str();
		                }
		            }
		            catch (...)
		            {
		            }

		            try
		            {
		                this->title = cfg_ini->get_string("title", "main");
		            }
		            catch (...)
		            {
		            }

		            try
		            {
						this->default_w = cfg_ini->get_int("w", "main");
		            }
		            catch (...)
		            {
		            }

		            try
		            {
						this->default_h = cfg_ini->get_int("h", "main");
		            }
		            catch (...)
		            {
		            }

		            try
		            {
						this->zoom = std::max(1.05f, cfg_ini->get_float("zoom", "main") );
		            }
		            catch (...)
		            {
		            }

		            try
		            {
		                std::string s_excl = cfg_ini->get_string("exclude_folder", "main");
						this->exclude_folder = Config::split(s_excl, ';');
		            }
		            catch (...)
		            {
		            }

		            try
		            {
		                std::string s_img = cfg_ini->get_string("img", "main");
						this->img = Config::split(s_img, ';');
		            }
		            catch (...)
		            {
		            }
		        }
		    }
			
	    }

		filesystem::path path_folder(this->path_dir);
		if ((path_folder.empty() == false) && (path_folder.exists() == true) && (path_folder.is_directory() == true))
		{
			return true;// ok
		}
		else
		{
			
			return false;
		}
	}

    static std::vector<std::string> split(const std::string &text, char sep)
    {
        std::vector<std::string> tokens;
        std::size_t start = 0, end = 0;
        while ((end = text.find(sep, start)) != std::string::npos)
        {
            tokens.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(text.substr(start));
        return tokens;
    }

    static std::string merge(std::vector<std::string> v)
    {
        std::string s;
        for (size_t i = 0; i<v.size(); i++)
        {
            s += v[i];
            if (i < v.size() - 1)
                s += ", ";
        }
        return s;
    }
};
