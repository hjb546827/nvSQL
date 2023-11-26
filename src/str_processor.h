/**
 * @file		str_processor.h
 * @brief
 * @author		hjb
 * @version		1.0
 * @date		2023-11-02
 * @copyright	Copyright (c) 2023
 */

#pragma once

#include <iostream>
#include <initializer_list>
#include <format>
#include <string>
#include <cstring>
#include <regex>

class str_processor
{
private:
protected:
public:
    str_processor() noexcept {}
    ~str_processor() noexcept {}

    /*字符串预处理*/
    void str_process(std::string &str)
    {
        // 全部转换为小写
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        // 清理多余制表符或空格
        std::regex tabs("\\t+");
        str = std::regex_replace(str, tabs, " ");
        std::regex spaces("\\s+");
        str = std::regex_replace(str, spaces, " ");
        if (str.at(0) == ' ')
        {
            str.erase(0, 1);
        }
        if (str.back() == ' ')
        {
            str.pop_back();
        }
    }
    /*字符串切割*/
    void str_split(std::string str, std::vector<std::string> &v, const std::string split)
    {
        char *s = new char[str.size() + 1];
        strcpy(s, str.c_str());
        char *p = strtok(s, split.c_str());

        while (p)
        {
            v.push_back(p);
            p = strtok(NULL, split.c_str());
        }

        delete[] s;
    }
    void str_split(std::string str, std::vector<std::string> &v, const char split)
    {
        str_split(str, v, std::string(1, split));
    }
    void str_split(std::string str, std::vector<std::string> &v, std::regex reg)
    {
        std::regex_token_iterator p(str.begin(), str.end(), reg, -1);
        decltype(p) end;
        for (; p != end; ++p)
        {
            v.push_back(p->str());
        }
    }
};