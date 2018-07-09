//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#pragma once

#include "filesystem/path.h"
#include "filesystem/resolver.h"
#include "ini_parser/ini_parser.hpp"
#include <iostream>
#include <stdio.h>
#include <atomic>
#include <future>
#include <cassert>

class UIState;

class FolderNavigation
{
public:
    UIState&                   _state;

    std::string                 path_dir;
    std::vector<std::string>    exclude_folder;
    std::vector<std::string>    img;

    filesystem::path            root;
    std::vector<std::string>    root_files;
    filesystem::path            current_parent;
    filesystem::path            current_path;

    FolderNavigation(UIState& st, const std::string& _path_dir, const std::vector<std::string>& _exclude_folder, const std::vector<std::string>& _img);
    ~FolderNavigation();

    void reset(const std::string& _path_dir);
  
    void load_root();   
    std::vector<std::string> get_img_files(filesystem::path& p);
    
    void next_path(bool no_deepening = false);
    void prev_path(bool no_deepening = false);
   
    filesystem::path find_next_folder(filesystem::path& parent_folder, filesystem::path& last_folder, bool no_deepening = false);
    filesystem::path find_last_folder(filesystem::path& parent_folder);
    filesystem::path find_prev_folder(filesystem::path& parent_folder, filesystem::path& last_folder, bool no_deepening = false);

    filesystem::path preview_next_path(bool no_deepening = false);

    static std::string select_folder(char const * const aDefaultPath);
};

