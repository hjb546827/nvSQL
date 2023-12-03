/**
 * @file        DQL.cpp
 * @brief       DQL
 * @author      hjb
 * @version     1.0
 * @date        2023-11-27
 * @copyright   Copyright (c) 2023
 */

#include "SQL.h"
#include <type_traits>

bool DQL::selectRecord(const std::string &database, const std::vector<std::string> &res, const std::string &cmd,
                       cache<table> &indexCache, CPUTimer &times) {
    std::vector<std::string> conditions, table_name, tmp1, tmp2;
    std::vector<tColumn> properties;
    std::vector<std::vector<std::string>> select_res;
    std::vector<int> max_num;
    str_split(cmd, conditions, std::regex("\\swhere\\s"));
    str_split(conditions[0], table_name, std::regex("\\sfrom\\s"));
    str_split(table_name[0], tmp1, std::regex("\\s?select\\s"));
    str_split(tmp1[1], tmp2, std::regex("\\s?,\\s?"));
    // select
    if (res[1] != "*") {
        for (auto i : tmp2) {
            tColumn tmp3;
            tmp3.name = i;
            tmp3.type = STRING;
            properties.push_back(tmp3);
        }
    }
    // where
    tCdtNameList_t cdts;
    if (cmd.find("where") + 1) {
        std::vector<std::string> w_tmp;
        std::replace(conditions[1].begin(), conditions[1].end(), '(', ' ');
        std::replace(conditions[1].begin(), conditions[1].end(), ')', ' ');
        str_process(conditions[1]);
        str_split(conditions[1], w_tmp, std::regex("\\s?,\\s?"));
        for (auto i = 0uz; i < w_tmp.size(); ++i) {
            std::vector<std::string> c_tmp;
            str_split(w_tmp[i], c_tmp, std::regex("\\s?(([><]=?)|=)\\s?"));
            char oper = arithOperMatch(w_tmp[i], c_tmp[0]);
            if (oper == 5) {
                std::cout << "Syntax error!" << std::endl;
                return false;
            }
            if (c_tmp[1].front() == '\"') {
                c_tmp[1].erase(0, 1);
                c_tmp[1].pop_back();
            }
            c_tmp[1].push_back(oper);
            cdts.emplace_back(c_tmp[0], c_tmp[1]);
        }
    }

    std::vector<int> widths;
    std::vector<std::string> props;
    std::vector<std::vector<std::string>> datas;
    if (res[1] != "*") {
        for (auto i : tmp2) {
            props.push_back(i);
        }
    }
    if (table<>::getKeyType(database, table_name[1]) == 0) { // int
        int tableID = -1;
        for (auto i = 0; i < (int)indexCache.iCaches.size(); ++i) {
            if (indexCache.iCaches[i].database == database && indexCache.iCaches[i].name == table_name[1]) {
                if (indexCache.last == i) {
                    indexCache.last = 3 - indexCache.last - indexCache.first;
                    indexCache.first = i;
                } else {
                    indexCache.first = i;
                }
                tableID = i;
                break;
            }
        }
        if (tableID == -1) {
            tableID = indexCache.last;
            indexCache.iCaches[tableID].renew();
            indexCache.iCaches[tableID].init(database, table_name[1]);
            indexCache.iCaches[tableID].openTable();
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<int> &t = indexCache.iCaches[tableID];
        if (!t.readTable(widths, props, datas, cdts)) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }
        times.end();
        draw_data(widths, props, datas);
    } else { // string
        int tableID = -1;
        for (auto i = 0; i < (int)indexCache.sCaches.size(); ++i) {
            if (indexCache.sCaches[i].database == database && indexCache.sCaches[i].name == table_name[1]) {
                if (indexCache.last == i) {
                    indexCache.last = 3 - indexCache.last - indexCache.first;
                    indexCache.first = i;
                } else {
                    indexCache.first = i;
                }
                tableID = i;
                break;
            }
        }
        if (tableID == -1) {
            tableID = indexCache.last;
            indexCache.sCaches[tableID].renew();
            indexCache.sCaches[tableID].init(database, table_name[1]);
            indexCache.sCaches[tableID].openTable();
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<std::string> &t = indexCache.sCaches[tableID];
        if (!t.readTable(widths, props, datas, cdts)) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }
        times.end();
        draw_data(widths, props, datas);
    }

    return true;
}