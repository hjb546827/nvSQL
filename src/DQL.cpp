/**
 * @file        DQL.cpp
 * @brief       DQL
 * @author      hjb
 * @version     1.0
 * @date        2023-11-27
 * @copyright   Copyright (c) 2023
 */

#include "DQL.h"

bool DQL::selectRecord(const std::string &database, const std::vector<std::string> &res, const std::string &cmd) {
    std::vector<std::string> conditions, table_name, tmp1, tmp2;
    std::vector<column_struct> properties;
    std::vector<std::vector<std::string>> select_res;
    std::vector<int> max_num;
    str_split(cmd, conditions, std::regex("\\swhere\\s"));
    str_split(conditions[0], table_name, std::regex("\\sfrom\\s"));
    str_split(table_name[0], tmp1, std::regex("\\s?select\\s"));
    str_split(tmp1[1], tmp2, std::regex("\\s?,\\s?"));
    // select
    if (res[1] != "*") {
        for (auto i : tmp2) {
            column_struct tmp3;
            tmp3.name = i;
            tmp3.type = STRING;
            properties.push_back(tmp3);
        }
    }
    // where
    std::vector<std::pair<std::string, std::string>> cdts;
    if (cmd.find("where") + 1) {
        std::vector<std::string> w_tmp;
        std::replace(conditions[1].begin(), conditions[1].end(), '(', ' ');
        std::replace(conditions[1].begin(), conditions[1].end(), ')', ' ');
        str_process(conditions[1]);
        str_split(conditions[1], w_tmp, std::regex("\\s?,\\s?"));
        for (auto i = 0uz; i < w_tmp.size(); ++i) {
            std::vector<std::string> c_tmp;
            str_split(w_tmp[i], c_tmp, std::regex("\\s?=\\s?"));
            if (c_tmp[1].front() == '\"') {
                c_tmp[1].erase(0, 1);
                c_tmp[1].pop_back();
            }
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
        table<int> t(database, table_name[1]);
        t.openTable();
        if (!t.readTable(widths, props, datas, cdts)) {
            cout << "Table not exists!" << endl;
            return false;
        }
        draw_data(widths, props, datas);
    } else { // string
        table<string> t(database, table_name[1]);
        t.openTable();
        if (!t.readTable(widths, props, datas, cdts)) {
            cout << "Table not exists!" << endl;
            return false;
        }
        draw_data(widths, props, datas);
    }

    return true;
}