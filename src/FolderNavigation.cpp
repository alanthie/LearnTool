//=================================================================================================
//                  Copyright (C) 2018 Alain Lanthier, Samuel Lanthier - All Rights Reserved  
//                  License: MIT License
//=================================================================================================
#pragma once

#include "FolderNavigation.h"
#include "UIState.h"
#include <iostream>
#include <stdio.h>
#include <atomic>
#include <future>
#include <cassert>

FolderNavigation::FolderNavigation(UIState& st, const std::string& _path_dir, const std::vector<std::string>& _exclude_folder, const std::vector<std::string>& _img) :
    _state(st),
    path_dir(_path_dir),
    exclude_folder(_exclude_folder),
    img(_img)
{
    //root = filesystem::path("..\\res\\topic");
    root            = filesystem::path(path_dir);
    root_files      = filesystem::path::get_directory_file(root, false, true);
    current_parent  = filesystem::path(root);
    current_path    = find_next_folder(root, filesystem::path());
}

void FolderNavigation::reset(const std::string& _path_dir)
{
    path_dir = _path_dir;
    root = filesystem::path(_path_dir);
    root_files = filesystem::path::get_directory_file(root, false, true);
    current_parent = filesystem::path(root);
    current_path = find_next_folder(root, filesystem::path());
}

FolderNavigation::~FolderNavigation()
{
}

void FolderNavigation::load_root()
{
    // Restart
    current_parent = filesystem::path(root);
    current_path = find_next_folder(root, filesystem::path());

    if (current_path.empty() == false)
    {
        _state.load_path(current_path);
    }
    else
    {
        assert(false);
    }
}


std::vector<std::string> FolderNavigation::get_img_files(filesystem::path& p)
{
    std::vector<std::string> imgfiles;
    std::vector<std::string> files = filesystem::path::get_directory_file(p, false);
    for (size_t i = 0; i < files.size(); i++)
    {
        filesystem::path pv = files.at(i);
        if (pv.is_file())
        {
            std::string s = pv.extension();
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            if (std::find(img.begin(), img.end(), s) != img.end())
            {
                imgfiles.push_back(pv.make_absolute().str());
            }
        }
    }
    return imgfiles;
}

void FolderNavigation::next_path(bool no_deepening)
{
    filesystem::path save_current_path = current_path;
    filesystem::path save_current_parent = save_current_path.parent_path();

    assert(save_current_parent.empty() == false);

    current_path = find_next_folder(save_current_parent, save_current_path, no_deepening);
    if (current_path.empty() == false)
    {
        if (current_parent != current_path.parent_path())
        {
            current_parent = current_path.parent_path();
        }

        if (current_path.empty() == false)
        {
            _state.load_path(current_path);
        }
        else
        {
            assert(false);
        }
    }
    else if (save_current_parent.make_absolute().str() == root.make_absolute().str())
    {
        // Restart
        load_root();
        return;
    }
    else
    {
        current_path = save_current_parent;
        current_parent = save_current_parent.parent_path();
        if (current_path.make_absolute().str() == root.make_absolute().str())
        {
            // Restart
            load_root();
            return;
        }

        current_path = find_next_folder(current_parent, current_path, true);
        while (current_path.empty() == true)
        {
            current_path = save_current_parent.parent_path();
            current_parent = save_current_parent.parent_path().parent_path();

            if (current_path.make_absolute().str() == root.make_absolute().str())
            {
                // Restart
                load_root();
                return;
            }

            next_path(true);
        }

        if (current_path.empty() == false)
        {
            _state.load_path(current_path);
        }
        else
        {
            assert(false);
        }
    }
}

void FolderNavigation::prev_path(bool no_deepening)
{
    filesystem::path save_current_path = current_path;
    filesystem::path save_current_parent = save_current_path.parent_path();

    current_path = find_prev_folder(save_current_parent, save_current_path, no_deepening);
    if (current_path.empty() == false)
    {
        current_parent = current_path.parent_path();
        if (current_path.make_absolute().str() == root.make_absolute().str())
        {
            // Restart
            load_root();
            return;
        }
        else
        {
            if (current_path.empty() == false)
            {
                _state.load_path(current_path);
            }
            else
            {
                assert(false);
            }
        }
    }
    else
    {
        current_path = save_current_parent;
        current_parent = save_current_parent.parent_path();

        if ((current_path.empty() == true) || (current_path.make_absolute().str() == root.make_absolute().str()))
        {
            // Restart
            load_root();
            return;
        }

        if (current_path.empty() == false)
        {
            current_parent = current_path.parent_path();
            _state.load_path(current_path);
        }
        else
        {
            assert(false);
        }
    }
}

filesystem::path FolderNavigation::find_next_folder(filesystem::path& parent_folder, filesystem::path& last_folder, bool no_deepening)
{
    filesystem::path p;
    if (last_folder.empty())
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
        for (size_t i = 0; i < v.size(); i++)
        {
            if (std::find(exclude_folder.begin(), exclude_folder.end(), filesystem::path(v[i]).filename()) == exclude_folder.end())
            {
                std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
                if (vf.size() > 0)
                {
                    p = filesystem::path(v[i]);
                    return p;
                }
                else
                {
                    // TODO...
                    std::vector<std::string> v_sub = filesystem::path::get_directory_file(filesystem::path(v[i]), false, true);
                    for (size_t j = 0; j < v_sub.size(); j++)
                    {
                        if (std::find(exclude_folder.begin(), exclude_folder.end(), filesystem::path(v_sub[j]).filename()) == exclude_folder.end())
                        {
                            std::vector<std::string> vf = get_img_files(filesystem::path(v_sub[j]));
                            if (vf.size() > 0)
                            {
                                p = filesystem::path(v_sub[j]);
                                return p;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
        int k = 0;
        for (int i = 0; i < v.size(); i++)
        {
            if (v[i] == last_folder.make_absolute().str())
            {
                k = i;
                break;
            }
        }

        if (no_deepening == false)
        {
            std::vector<std::string> v_sub = filesystem::path::get_directory_file(last_folder, false, true);
            for (size_t j = 0; j < v_sub.size(); j++)
            {
                if (std::find(exclude_folder.begin(), exclude_folder.end(), filesystem::path(v_sub[j]).filename()) == exclude_folder.end())
                {
                    std::vector<std::string> vf_sub = get_img_files(filesystem::path(v_sub[j]));
                    if (vf_sub.size() > 0)
                    {
                        p = filesystem::path(v_sub[j]);
                        return p;
                    }
                }
            }
        }

        for (int i = k + 1; i < v.size(); i++)
        {
            if (std::find(exclude_folder.begin(), exclude_folder.end(), filesystem::path(v[i]).filename()) == exclude_folder.end())
            {
                std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
                if (vf.size() > 0)
                {
                    p = filesystem::path(v[i]);
                    break;
                }
            }

            std::vector<std::string> v_sub = filesystem::path::get_directory_file(filesystem::path(v[i]), false, true);
            for (size_t j = 0; j < v_sub.size(); j++)
            {
                if (std::find(exclude_folder.begin(), exclude_folder.end(), filesystem::path(v_sub[j]).filename()) == exclude_folder.end())
                {
                    std::vector<std::string> vf_sub = get_img_files(filesystem::path(v_sub[j]));
                    if (vf_sub.size() > 0)
                    {
                        p = filesystem::path(v_sub[j]);
                        return p;
                    }
                }
            }
        }

    }

    return p;
}

filesystem::path FolderNavigation::find_last_folder(filesystem::path& parent_folder)
{
    filesystem::path p;

    std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
    for (size_t i = v.size() - 1; i >= 0; i--)
    {
        if (std::find(exclude_folder.begin(), exclude_folder.end(), filesystem::path(v[i]).filename()) == exclude_folder.end())
        {
            std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
            if (vf.size() > 0)
            {
                p = filesystem::path(v[i]);
                break;
            }
        }
    }
    return p;
}

filesystem::path FolderNavigation::find_prev_folder(filesystem::path& parent_folder, filesystem::path& last_folder, bool no_deepening)
{
    filesystem::path p;
    if (last_folder.empty())
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
        for (size_t i = 0; i < v.size(); i++)
        {
            if (std::find(exclude_folder.begin(), exclude_folder.end(), filesystem::path(v[i]).filename()) == exclude_folder.end())
            {
                std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
                if (vf.size() > 0)
                {
                    p = filesystem::path(v[i]);
                    break;
                }
            }
        }
    }
    else
    {
        std::vector<std::string> v = filesystem::path::get_directory_file(parent_folder, false, true);
        int k = 0;
        for (int i = 0; i < v.size(); i++)
        {
            if (v[i] == last_folder.make_absolute().str())
            {
                k = i;
                break;
            }
        }

        for (int i = k - 1; i >= 0; i--)
        {
            if (std::find(exclude_folder.begin(), exclude_folder.end(), filesystem::path(v[i]).filename()) == exclude_folder.end())
            {
                std::vector<std::string> vf = get_img_files(filesystem::path(v[i]));
                if (vf.size() > 0)
                {
                    p = filesystem::path(v[i]);
                    break;
                }
            }
        }

        // at root
    }

    return p;
}


filesystem::path FolderNavigation::preview_next_path(bool no_deepening)
{
    filesystem::path ret_path;
    filesystem::path save_current_path      = current_path;
    filesystem::path save_current_parent    = save_current_path.parent_path();

    assert(save_current_parent.empty() == false);

    current_path = find_next_folder(save_current_parent, save_current_path, no_deepening);
    if (current_path.empty() == false)
    {
        if (current_parent != current_path.parent_path())
        {
            current_parent = current_path.parent_path();
        }

        if (current_path.empty() == false)
        {
            ret_path        = current_path;
            current_path    = save_current_path;
            current_parent = save_current_parent;
            return ret_path;
        }
        else
        {
            assert(false);
        }
    }
    else if (save_current_parent.make_absolute().str() == root.make_absolute().str())
    {
        // Restart
        //load_root();
        ret_path        = find_next_folder(root, filesystem::path());
        current_path    = save_current_path;
        current_parent  = save_current_parent;
        return ret_path;
    }
    else
    {
        current_path = save_current_parent;
        current_parent = save_current_parent.parent_path();
        if (current_path.make_absolute().str() == root.make_absolute().str())
        {
            // Restart
            //load_root();
            ret_path        = find_next_folder(root, filesystem::path());
            current_path    = save_current_path;
            current_parent  = save_current_parent;
            return ret_path;
        }

        current_path = find_next_folder(current_parent, current_path, true);
        while (current_path.empty() == true)
        {
            current_path = save_current_parent.parent_path();
            current_parent = save_current_parent.parent_path().parent_path();

            if (current_path.make_absolute().str() == root.make_absolute().str())
            {
                // Restart
                //load_root();
                ret_path        = find_next_folder(root, filesystem::path());
                current_path    = save_current_path;
                current_parent  = save_current_parent;
                return ret_path;
            }

            ret_path        = preview_next_path(true);
            current_path    = save_current_path;
            current_parent = save_current_parent;
            return ret_path;
        }

        if (current_path.empty() == false)
        {
            ret_path        = current_path;
            current_path    = save_current_path;
            current_parent  = save_current_parent;
            return ret_path;
        }
        else
        {
            assert(false);
        }
    }
    return ret_path;
}