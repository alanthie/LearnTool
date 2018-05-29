//
#pragma once
#include <string>

class Config
{
public:
    std::string title = "Learning Tool";
    int default_w = 900;
    int default_h = 600;
    std::string path_dir = ".\\";

    Config() = default;
};
