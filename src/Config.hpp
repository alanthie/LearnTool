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
    float zoom = 1.25;
    std::vector<std::string> exclude_folder = { ".Thumbs" };
    std::vector<std::string> img = { "jpg",  "png", "gif", "jpeg", "bmp" };

    Config() = default;

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
