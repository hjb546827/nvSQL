/**
 * @file		base.h
 * @brief
 * @author		hjb
 * @version		1.0
 * @date		2023-11-02
 * @copyright	Copyright (c) 2023
 */

#pragma once

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <format>
#include <regex>
#include "data_struct.h"

/**
 * @brief	字符串预处理
 * @param	str
 */
inline void str_process(std::string &str)
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

/**
 * @brief	字符串分割
 * @param	str     待分割字符串
 * @param	v       输出的字符串组
 * @param	split   用于分割的字符串
 */
inline void str_split(std::string str, std::vector<std::string> &v, const std::string split)
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
/**
 * @brief	字符串分割
 * @param	str     待分割字符串
 * @param	v       输出的字符串组
 * @param	reg     用于分割的字符串（正则表达式形式表示）
 */
inline void str_split(std::string str, std::vector<std::string> &v, std::regex reg)
{
    std::regex_token_iterator p(str.begin(), str.end(), reg, -1);
    decltype(p) end;
    for (; p != end; ++p)
    {
        v.push_back(p->str());
    }
}
/**
 * @brief	字符串分割
 * @param	str     待分割的字符串
 * @param	v       输出的字符串组
 * @param	split   用于分割的字符
 */
inline void str_split(std::string str, std::vector<std::string> &v, const char split)
{
    str_split(str, v, std::string(1, split));
}

inline bool searchDatabase(const std::string database){
    if (!std::filesystem::exists("data/" + database)) {
        std::cout << "Database not exists!" << std::endl;
        return false;
    } else
        return true;
}

inline bool searchTable(const std::string database, const std::string tablename) {
    if (!std::filesystem::exists("data/" + database + "/" + tablename + ".prof")) {
        std::cout << "Table not exists!" << std::endl;
        return false;
    } else
        return true;
}

/**
 * @brief	打印行线
 * @param	max_num 每列最大宽度
 * @param	column  列数
 */
inline void draw_line(std::vector<int> max_num, int column) {
    for (auto i = 0; i < column; ++i) {
        std::cout << "+-";
        for (auto j = 0; j < max_num[i] + 1; ++j) {
            std::cout << "-";
        }
    }
    std::cout << "+" << std::endl;
}
/**
 * @brief	打印表
 * @param	max_num 每列最大宽度
 * @param	prop    表属性
 * @param	data    表数据
 */
inline void draw_data(std::vector<int> &max_num, std::vector<std::string> &prop, std::vector<std::vector<std::string>> &data) {
    if (prop.size() < 1) {
        std::cout << "Table is empty!" << std::endl;
    }
    draw_line(max_num, prop.size());
    for (auto i = 0uz; i < prop.size(); ++i) {
        std::cout << "| " << std::setw(max_num[i]) << std::setiosflags(std::ios::left) << prop[i] << " ";
    }
    std::cout << "|" << std::endl;
    for (auto i = 0uz; i < data.size(); ++i) {
        draw_line(max_num, prop.size());
        for (auto j = 0uz; j < prop.size(); ++j) {
            std::cout << "| " << std::setw(max_num[j]) << std::setiosflags(std::ios::left) << data[i][j] << " ";
        }
        std::cout << "|" << std::endl;
    }
    draw_line(max_num, prop.size());
}
/**
 * @brief	打印表
 * @param	max_num 每列最大宽度
 * @param	prop    表属性
 * @param	data    表数据
 */
inline void draw_data(std::vector<int> &max_num, std::vector<column_struct> &prop, std::vector<std::vector<std::string>> &data) {
    std::vector<std::string> tmp_prop;
    for (auto i : prop) {
        tmp_prop.push_back(i.name);
    }
    draw_data(max_num, tmp_prop, data);
}