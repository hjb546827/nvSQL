/**
 * @file        utility.h
 * @brief
 * @author      hjb
 * @version     1.0
 * @date        2023-11-02
 * @copyright   Copyright (c) 2023
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <format>
#include <chrono>
#include <regex>
#include <string>
#include <vector>
#include "tData.h"

// 单个表查询属性名与查询值
using tCdtName_t = std::pair<std::string, std::string>;
// 多个表查询属性名与查询值
using tCdtNameList_t = std::vector<tCdtName_t>;
// 单个表查询属性在属性列表中位置与查询值
using tCdtPos_t = std::pair<int, std::string>;
// 多个表查询属性在属性列表中位置与查询值
using tCdtPosList_t = std::vector<tCdtPos_t>;
// 单个表属性名称与类型
using tPropType_t = std::pair<std::string, char>;
// 多个表属性名称与类型
using tPropTypeList_t = std::vector<tPropType_t>;
// 用于打印的数据结构
using printData_t = std::vector<std::vector<std::string>>;

/**
 * @brief   字符串预处理
 * @param   str
 */
inline void str_process(std::string &str) {
    // 全部转换为小写
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    // 清理多余制表符或空格
    std::regex tabs("\\t+");
    str = std::regex_replace(str, tabs, " ");
    std::regex spaces("\\s+");
    str = std::regex_replace(str, spaces, " ");
    if (str.at(0) == ' ') {
        str.erase(0, 1);
    }
    if (str.back() == ' ') {
        str.pop_back();
    }
}

/**
 * @brief   字符串分割
 * @param   str     待分割字符串
 * @param   v       输出的字符串组
 * @param   split   用于分割的字符串
 */
inline void str_split(std::string str, std::vector<std::string> &v, const std::string split) {
    char *s = new char[str.size() + 1];
    strcpy(s, str.c_str());
    char *p = strtok(s, split.c_str());

    while (p) {
        v.push_back(p);
        p = strtok(NULL, split.c_str());
    }

    delete[] s;
}
/**
 * @brief   字符串分割
 * @param   str     待分割字符串
 * @param   v       输出的字符串组
 * @param   reg     用于分割的字符串（正则表达式形式表示）
 */
inline void str_split(std::string str, std::vector<std::string> &v, std::regex reg) {
    std::regex_token_iterator p(str.begin(), str.end(), reg, -1);
    decltype(p) end;
    for (; p != end; ++p) {
        v.push_back(p->str());
    }
}
/**
 * @brief   字符串分割
 * @param   str     待分割的字符串
 * @param   v       输出的字符串组
 * @param   split   用于分割的字符
 */
inline void str_split(std::string str, std::vector<std::string> &v, const char split) {
    str_split(str, v, std::string(1, split));
}

/**
 * @brief   判断where子句中的大小比较运算符
 * @param   s   where子句中某一句的原字符串
 * @param   s1  原字符串被切割后的第一部分
 * @return  uint8_t
 */
inline uint8_t arithOperMatch(std::string &s, std::string &s1) {
    // > : 0; < : 1; = : 2; >= : 3; <= : 4;
    size_t fsz = s1.size();
    char oper = s[fsz];
    if (oper == ' ') {
        fsz++;
        oper = s[fsz];
    }
    switch (oper) {
    case '>':
        if (s[fsz + 1] == '=') {
            return 3;
        } else {
            return 0;
        }
        break;
    case '<':
        if (s[fsz + 1] == '=') {
            return 4;
        } else {
            return 1;
        }
        break;
    case '=':
        return 2;
        break;
    }
    return 5;
}

/**
 * @brief   检测数据库是否存在
 * @param   database
 * @return  true
 * @return  false
 */
inline bool searchDatabase(const std::string database) {
    if (!std::filesystem::exists("data/" + database)) {
        std::cout << "Database not exists!" << std::endl;
        return false;
    } else
        return true;
}

/**
 * @brief   检测表是否存在
 * @param   database
 * @param   tablename
 * @return  true
 * @return  false
 */
inline bool searchTable(const std::string database, const std::string tablename) {
    if (!std::filesystem::exists("data/" + database + "/" + tablename + ".prof")) {
        std::cout << "Table not exists!" << std::endl;
        return false;
    } else
        return true;
}

/**
 * @brief   打印行线
 * @param   max_num 每列最大宽度
 * @param   column  列数
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
 * @brief   打印表
 * @param   max_num 每列最大宽度
 * @param   prop    表属性
 * @param   data    表数据
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
        if (data[i].size() == 0uz) {
            continue;
        }
        draw_line(max_num, prop.size());
        for (auto j = 0uz; j < prop.size(); ++j) {
            std::cout << "| " << std::setw(max_num[j]) << std::setiosflags(std::ios::left) << data[i][j] << " ";
        }
        std::cout << "|" << std::endl;
    }
    draw_line(max_num, prop.size());
}
/**
 * @brief   打印表
 * @param   max_num 每列最大宽度
 * @param   prop    表属性
 * @param   data    表数据
 */
inline void draw_data(std::vector<int> &max_num, std::vector<tColumn> &prop, std::vector<std::vector<std::string>> &data) {
    std::vector<std::string> tmp_prop;
    for (auto i : prop) {
        tmp_prop.push_back(i.name);
    }
    draw_data(max_num, tmp_prop, data);
}

template <template <typename> class Cont>
struct cache {
    std::vector<Cont<int>> iCaches;
    std::vector<Cont<std::string>> sCaches;
    int first;
    int last;

    cache() {
        first = 2;
        last = 0;
        for (auto i = 0; i < 3; ++i) {
            iCaches.push_back(Cont<int> {});
            sCaches.push_back(Cont<std::string> {});
        }
    }
};

/**
 * @brief   计时器
 */
class CPUTimer {
private:
    using _clock = std::chrono::high_resolution_clock;
    using _timePoint = std::chrono::time_point<_clock>;

    _timePoint _start, _end;
    std::chrono::microseconds _duration;
public:
    CPUTimer() = default;
    ~CPUTimer() {}

    /**
     * @brief   开始计时
     */
    void start() {
        _start = std::chrono::system_clock::now();
    }

    /**
     * @brief   停止计时
     */
    void end() {
        _end = std::chrono::system_clock::now();
        _duration = std::chrono::duration_cast<std::chrono::microseconds>(_end - _start);
    }

    /**
     * @brief   打印耗时
     */
    void print() {
        std::cout << "costs " << double(_duration.count()) / 1000.0f << "ms" << std::endl;
    }

    /**
     * @brief	获取时间间隔
     * @return	std::string 
     */
    std::string get_duration(){
        return std::format("{}ms", double(_duration.count()) / 1000.0f);
    }
};