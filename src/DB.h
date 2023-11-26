/**
 * @file        DB.h
 * @brief
 * @author      hjb
 * @version     1.0
 * @date        2023-11-02
 * @copyright   Copyright (c) 2023
 */

#pragma once

#include <iostream>
#include <initializer_list>
#include <format>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include "fsystem.h"
#include "data_struct.h"
#include "str_processor.h"
#include "table.h"

class DB {
private:
    std::string name = "db"; // 当前选中数据库
    std::string cmd = "";    // 用户指令
    std::string tmp = "";    // 用户上一条指令
    int flag = 0;            // 循环控制
    int cmd_end = 1;         // 语句结束判断
    int paren_end = 1;       // 括号匹配

    fsystem f; // 文件操作
    str_processor sp;

    /*主循环*/
    void update() {
        if (paren_end == 0)
            cmd_end = 0;
        if (cmd_end == 1) // 正常显示
            std::cout << name << "> ";
        if (cmd_end == 0) {
            // 用户上一条指令未结束
            for (auto i = 0uz; i < name.size() - 1; ++i) {
                std::cout << " ";
            }
            std::cout << "-> ";
        }
        if (std::getline(std::cin, cmd)) {
            // 读取用户指令
            if (cmd.size() < 1)
                return;
            sp.str_process(cmd); // 指令预处理
            sql_process(cmd);    // 语句解析
        }
    }
    /*create语句*/
    bool create_db(const char *db) {
        return f.create_db(db);
    }
    bool create_table(const char *db, const char *table, const std::vector<int> &ints, const std::vector<std::string> &ids) {
        return f.create_table(db, table, ints, ids);
    }
    bool create_table(const char *db, const char *table, const std::vector<column_struct> &data) {
        return f.create_table(db, table, data);
    }
    /*drop语句*/
    bool drop_db(const char *db) {
        return f.drop_db(db);
    }
    bool drop_table(const char *db, const char *table) {
        return f.drop_table(db, table);
    }
    /*insert语句*/
    bool insert_table(const char *db, const char *table, std::vector<row_struct> &data) {
        return f.insert_table(db, table, data);
    }
    /*select语句*/
    bool select_table(const char *db, const char *table, std::vector<column_struct> &data,
                      std::vector<std::vector<std::string>> &res, std::vector<int> &max_num, std::multimap<std::string, std::string> &cdt) {
        return f.select_table(db, table, data, res, max_num, cdt);
    }
    /*delete语句*/
    bool delete_record(const char *db, const char *table, std::multimap<std::string, std::string> _map) {
        return f.delete_record(db, table, _map);
    }
    /*语句处理*/
    void sql_process(std::string &cmd) {
        std::string s_test = tmp + " " + cmd;
        if (std::count(s_test.begin(), s_test.end(), '(') != std::count(s_test.begin(), s_test.end(), ')')) {
            paren_end = 0;
            cmd_end = 0;
        } else {
            paren_end = 1;
        }
        if (cmd.find(";") + 1 && paren_end == 0) {
            // 括号不匹配
            std::cout << "Syntax error!(Parens mismatch)" << std::endl;
            paren_end = 1;
            cmd_end = 1;
            tmp = "";

            return;
        }
        if (cmd.find(";") + 1 && paren_end == 1) {
            // 语句结束
            if (cmd_end == 1 && cmd.size() == 1) // 空语句
                return;
            cmd_end = 1;

            if (tmp.size() > 0)
                tmp = tmp + " ";

            cmd = tmp + cmd; // 合成新语句
            tmp = "";
#ifdef DEBUG
            std::cout << "cmd: " << cmd << std::endl;
#endif

            if (cmd == "exit;") {
                // 退出指令
                flag = 1;
            } else if (cmd.size() > 0) {
                // 语句解析
                cmd.pop_back(); // 去除末尾分号
                sp.str_process(cmd);

                std::vector<std::string> res;
                sp.str_split(cmd, res, " "); // 拆解语句
                sql_execute(res, cmd);       // 语句执行
            } else {
                // 无法解析
                std::cout << "Syntax error!" << std::endl;
                paren_end = 1;
                cmd_end = 1;
                tmp = "";

                return;
            }
        } else {
            // 语句未结束
            cmd_end = 0;
            if (tmp.size() > 0)
                tmp = tmp + " ";

            tmp = tmp + cmd;

            if (tmp == "exit") {
                flag = 1;
            }
        }
    }
    /*sql语句执行*/
    bool sql_execute(const std::vector<std::string> &res, const std::string &cmd) {
        bool status = true;
        // create xxx xxx
        {
            // create database
            std::string create_database_xxx = "create\\sdatabase\\s[a-zA-Z]+[a-zA-Z0-9]*";
            std::string create_database_regex = "^\\s?" + create_database_xxx + "\\s?" + "$";
            if (std::regex_match(cmd, std::regex(create_database_regex))) {
                // 正则表达式匹配
                if (create_db(res[2].c_str())) {
                    std::cout << "Create database successfully!" << std::endl;
                } else {
                    status &= false;
                }
                return status;
            }
        }
        {
            // create table
            std::string create_table_xxx = "create\\stable\\s[a-zA-Z]+[a-zA-Z0-9]*";
            std::string name_type_constraint = "[a-zA-Z]+[a-zA-Z0-9]*\\s(int|string)\\s?(\\sprimary)?";
            std::string create_table_regex = "^\\s?" + create_table_xxx + "\\s?\\(\\s?(" +
                    name_type_constraint + "\\s?,\\s?" + ")*(" + name_type_constraint + "\\s?,?\\s?)" + "\\)\\s?" + "$";
            if (std::regex_match(cmd.c_str(), std::regex(create_table_regex))) {
                // 正则表达式匹配
                std::string table_name = res[2];
                std::vector<std::string> split_res;
                std::vector<column_struct> data;
                std::string content = cmd.substr(cmd.find("(") + 1, cmd.find(")") - cmd.find("(") - 1);
                std::vector<column_struct> primaryKey;
                int pkPos = 0;
                sp.str_process(content);
                sp.str_split(content, split_res, std::regex("\\s?,\\s?"));
#ifdef DEBUG
                std::cout << "content: [" << content << "]" << std::endl;
#endif
                for (auto i = 0uz; i < split_res.size(); ++i) {
                    if (split_res[i].front() == ' ')
                        split_res[i].erase(0, 1);
                    column_struct d;
                    std::vector<std::string> s_tmp;
                    sp.str_split(split_res[i], s_tmp, " ");
                    if (s_tmp.size() > 1) {
                        d.name = s_tmp[0];
                        if (s_tmp[1] == "int")
                            d.type = INT;
                        if (s_tmp[1] == "string")
                            d.type = STRING;
                        if (s_tmp.size() == 3 && s_tmp[2] == "primary")
                            d.is_primary = true;
                    }
                    data.push_back(d);
                    if(d.is_primary){
                        primaryKey.push_back(d);
                        pkPos = i;
                    }
                }
#ifdef DEBUG
                for (auto i : data) {
                    i.print_data();
                }
#endif
                if (table_name.find("(") + 1) {
                    while (table_name.back() != '(') {
                        table_name.pop_back();
                    }
                    table_name.pop_back();
                }
                
                if(primaryKey.size() > 1){
                    std::cout << "syntax error!" << endl;
                    status &= false;
                    return status;
                }
                if(primaryKey.size() == 0 && data.size() > 0){
                    primaryKey.push_back(data.front());
                    pkPos = 0;
                }
                vector<pair<string, char>> tProps;
                for(auto i : data){
                    tProps.push_back({i.name, 1 - (char)i.type});
                }
                if(primaryKey.front().type == 0){ // int
                    table<int> t(name, table_name);
                    t.createTable(tProps, pkPos);
                }
                else{ // string
                    table<string> t(name, table_name);
                    t.createTable(tProps, pkPos);
                }

                if (create_table(name.c_str(), table_name.c_str(), data)) {
                    std::cout << "Create table successfully!" << std::endl;
                } else {
                    status &= false;
                }
                return status;
            }
        }

        // insert xxx xxx
        {
            // insert xxx values (xxx)
            std::string insert_table_xxx = "insert\\s[a-zA-Z]+[a-zA-Z0-9]*\\svalues";
            std::string tuple_content = "(-?\\d+|\".*\")";
            std::string insert_table_regex = "^\\s?" + insert_table_xxx + "\\s?\\(\\s?(" +
                                             tuple_content + "\\s?,\\s?)*" + tuple_content + "\\s?,?" + "\\s?\\)\\s?" + "$";
            if (std::regex_match(cmd, std::regex(insert_table_regex))) {
                // 正则表达式匹配
                std::string table_name = res[1];
                std::vector<std::string> split_res;
                std::vector<row_struct> data;
                std::string content = cmd.substr(cmd.find("(") + 1, cmd.find(")") - cmd.find("(") - 1);
                sp.str_process(content);
                sp.str_split(content, split_res, std::regex("\\s?,\\s?"));
#ifdef DEBUG
                std::cout << "content: [" << content << "]" << std::endl;
#endif
                for (auto i : split_res) {
                    if (i.front() == ' ')
                        i.erase(0, 1);
                    row_struct d;
                    std::smatch res;
                    if (std::regex_match(i, res, std::regex("-?\\d+"))) {
                        d.type = INT;

                        // string -> int
                        int i_value = 0;
                        std::istringstream ss(res[0]);
                        ss >> i_value;

                        d.i_value = i_value;
                    } else if (std::regex_match(i, res, std::regex("\".+\""))) {
                        d.type = STRING;

                        d.s_value = res[0];

                        // 删除双引号
                        d.s_value.erase(0, 1);
                        d.s_value.pop_back();
                    }

                    data.push_back(d);
                }
#ifdef DEBUG
                for (auto i : data) {
                    i.print_data();
                }
#endif
                if(getKeyType(name, table_name) == 0){ // int
                    table<int> t(name, table_name);
                    t.openTable();
                    auto pk = t.getPrimaryKey();
                    t.insert({data[pk].i_value, content});
                }
                else{ // string
                    table<std::string> t(name, table_name);
                    t.openTable();
                    auto pk = t.getPrimaryKey();
                    t.insert({data[pk].s_value, content});
                }

                if (insert_table(name.c_str(), table_name.c_str(), data))
                    std::cout << "Insert table successfully!" << std::endl;
                else
                    status &= false;

                return status;
            }
        }

        // use xxx
        {
            // use xxx
            std::string use_database_xxx = "use\\s[a-zA-Z]+[a-zA-Z0-9]*";
            std::string use_database_regex = "^\\s?" + use_database_xxx + "\\s?" + "$";
            if (std::regex_match(cmd, std::regex(use_database_regex))) {
                // 正则表达式匹配
                if (f.search_db(res[1].c_str())) {
                    name = res[1];
                    std::cout << "Change database successfully!" << std::endl;
                } else {
                    status &= false;
                }
                return status;
            }
        }

        // select xxx from xxx [where xxx = xxx]
        {
            // select xxx from xxx
            std::string property_name = "([a-zA-Z]+[a-zA-Z0-9]*\\s?,\\s?)*[a-zA-Z]+[a-zA-Z0-9]*";
            std::string simple_condition = "([a-zA-Z]+[a-zA-Z0-9]*\\s?=\\s?((-?\\d+)|(\".*\")))";
            std::string paren_condition = std::format("(\\(\\s?{}\\s?\\))", simple_condition);
            std::string single_condition = std::format("({0}|{1})", simple_condition, paren_condition);
            std::string mutiply_condition = std::format("({0}|({0}\\s?,\\s?)*{0}|\\(\\s?({0}\\s?,\\s?)*{0}\\s?\\))", single_condition);
            std::string where_xxx = std::format("(\\swhere\\s{})?", mutiply_condition);
            std::string select_xxx = std::format("select\\s({}|\\*)\\sfrom\\s[a-zA-Z]+[a-zA-Z0-9]*", property_name);
            std::string select_regex = "^\\s?" + select_xxx + where_xxx + "\\s?" + "$";
            if (std::regex_match(cmd, std::regex(select_regex))) {
                // 正则表达式匹配
                std::string data = "";
                try {
                    std::vector<std::string> conditions, table_name, tmp1, tmp2;
                    std::vector<column_struct> properties;
                    std::vector<std::vector<std::string>> select_res;
                    std::vector<int> max_num;
                    sp.str_split(cmd, conditions, std::regex("\\swhere\\s"));
                    sp.str_split(conditions[0], table_name, std::regex("\\sfrom\\s"));
                    sp.str_split(table_name[0], tmp1, std::regex("\\s?select\\s"));
                    sp.str_split(tmp1[1], tmp2, std::regex("\\s?,\\s?"));
                    if (res[1] != "*") {
                        for (auto i : tmp2) {
                            column_struct tmp3;
                            tmp3.name = i;
                            tmp3.type = STRING;
                            properties.push_back(tmp3);
                        }
                    }
                    std::multimap<std::string, std::string> cdt;
                    std::vector<std::pair<std::string, std::string>> cdts;
                    if (cmd.find("where") + 1) {
                        std::vector<std::string> w_tmp;
                        std::replace(conditions[1].begin(), conditions[1].end(), '(', ' ');
                        std::replace(conditions[1].begin(), conditions[1].end(), ')', ' ');
                        sp.str_process(conditions[1]);
                        sp.str_split(conditions[1], w_tmp, std::regex("\\s?,\\s?"));
                        for (auto i = 0uz; i < w_tmp.size(); ++i) {
                            std::vector<std::string> c_tmp;
                            bool c_find = false;
                            sp.str_split(w_tmp[i], c_tmp, std::regex("\\s?=\\s?"));
                            if (c_tmp[1].front() == '\"') {
                                c_tmp[1].erase(0, 1);
                                c_tmp[1].pop_back();
                            }
                            // for(auto j : properties){
                            //     if(j.name == c_tmp[0]){
                            //         c_find = true;
                            //         break;
                            //     }
                            // }
                            // if(c_find == false){
                            //     std::cout << "Unknown selected column!" << std::endl;
                            //     return false;
                            // }
                            cdt.insert(std::multimap<std::string, std::string>::value_type(c_tmp[0], c_tmp[1]));
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
                    if(getKeyType(name, table_name[1]) == 0){ // int
                        table<int> t(name, table_name[1]);
                        t.openTable();
                        if(!t.readAll(widths, props, datas, cdts)){
                            cout << "not found!" << endl;
                            status &= false;
                            return status;
                        }
                        draw_data(widths, props, datas);
                    }
                    else{ // string
                        table<string> t(name, table_name[1]);
                        t.openTable();
                        if(!t.readAll(widths, props, datas, cdts)){
                            cout << "not found!" << endl;
                            status &= false;
                            return status;
                        }
                        draw_data(widths, props, datas);
                    }
                    
                    // if (select_table(name.c_str(), table_name[1].c_str(), properties, select_res, max_num, cdt)) {
                    //     draw_data(max_num, properties, select_res);
                    // }
                } catch (std::out_of_range e) {
                    status &= false;
                    std::cout << "Syntax error!" << std::endl;
                }
                return status;
            }
        }

        // delete xxx [where xxx]
        {
            // delete xxx
            std::string delete_table = "delete\\s[a-zA-Z]+[a-zA-Z0-9]*";
            std::string simple_condition = "([a-zA-Z]+[a-zA-Z0-9]*\\s?=\\s?((-?\\d+)|(\".*\")))";
            std::string paren_condition = std::format("(\\(\\s?{}\\s?\\))", simple_condition);
            std::string single_condition = std::format("({0}|{1})", simple_condition, paren_condition);
            std::string mutiply_condition = std::format("({0}|({0}\\s?,\\s?)*{0}|\\(\\s?({0}\\s?,\\s?)*{0}\\s?\\))", single_condition);
            std::string where_xxx = std::format("(\\swhere\\s{})?", mutiply_condition);
            std::string delete_table_regex = "^\\s?" + delete_table + where_xxx + "\\s?" + "$";
            try {
                if (std::regex_match(cmd, std::regex(delete_table_regex))) {
                    // 正则表达式匹配
                    if (f.search_table(name.c_str(), res[1].c_str())) {
                        std::vector<std::string> conditions;
                        sp.str_split(cmd, conditions, std::regex("\\swhere\\s"));
                        std::multimap<std::string, std::string> cdt;
                        std::vector<std::pair<std::string, std::string>> cdts;
                        if (cmd.find("where") + 1) {
                            std::vector<std::string> w_tmp;
                            std::replace(conditions[1].begin(), conditions[1].end(), '(', ' ');
                            std::replace(conditions[1].begin(), conditions[1].end(), ')', ' ');
                            sp.str_process(conditions[1]);
                            sp.str_split(conditions[1], w_tmp, std::regex("\\s?,\\s?"));
                            for (auto i = 0uz; i < w_tmp.size(); ++i) {
                                std::vector<std::string> c_tmp;
                                bool c_find = false;
                                sp.str_split(w_tmp[i], c_tmp, std::regex("\\s?=\\s?"));
                                if (c_tmp[1].front() == '\"') {
                                    c_tmp[1].erase(0, 1);
                                    c_tmp[1].pop_back();
                                }
                                cdt.insert(std::multimap<std::string, std::string>::value_type(c_tmp[0], c_tmp[1]));
                                cdts.emplace_back(c_tmp[0], c_tmp[1]);
                            }
                        }

                        if(getKeyType(name, res[1]) == 0){ // int
                            table<int> t(name, res[1]);
                            t.openTable();
                            t.eraseAll(cdts);
                        }
                        else{ // string
                            table<std::string> t(name, res[1]);
                            t.openTable();
                            t.eraseAll(cdts);
                        }

                        std::multimap<int, std::string> _map;
                        if (delete_record(name.c_str(), res[1].c_str(), cdt)) {
                            std::cout << "Delete record successfully!" << std::endl;
                        } else {
                            status &= false;
                        }
                    } else {
                        status &= false;
                    }
                    return status;
                }
            } catch (std::out_of_range e) {
                status &= false;
                std::cout << "Syntax error!" << std::endl;
                return status;
            }
        }

        // update xxx set xxx [where xxx]
        {
            // update xxx set xxx
            std::string update_xxx = "update\\s[a-zA-Z]+[a-zA-Z0-9]*\\sset\\s";
            std::string simple_condition = "([a-zA-Z]+[a-zA-Z0-9]*\\s?=\\s?((-?\\d+)|(\".*\")))";
            std::string paren_condition = std::format("(\\(\\s?{}\\s?\\))", simple_condition);
            std::string single_condition = std::format("({0}|{1})", simple_condition, paren_condition);
            std::string mutiply_condition = std::format("({0}|({0}\\s?,\\s?)*{0}|\\(\\s?({0}\\s?,\\s?)*{0}\\s?\\))", single_condition);
            std::string where_xxx = std::format("(\\swhere\\s{})?", mutiply_condition);
            std::string update_regex = "^\\s?" + update_xxx + simple_condition + where_xxx + "\\s?" + "$";
            if (std::regex_match(cmd, std::regex(update_regex))) {
                // 正则表达式匹配
                if (f.search_table(name.c_str(), res[1].c_str())) {
                    std::vector<std::string> conditions;
                    sp.str_split(cmd, conditions, std::regex("\\swhere\\s"));
                    // set
                    std::vector<std::string> set_values, values;
                    sp.str_split(conditions[0], set_values, std::regex("set\\s?"));
                    sp.str_split(set_values[1], values, std::regex("\\s?=\\s?"));
                    for (auto &i : values) {
                        if (i.front() == '\"') {
                            i.erase(0, 1);
                            i.pop_back();
                        }
                    }
                    std::pair<std::string, std::string> val{values[0], values[1]};
                    // where
                    std::multimap<std::string, std::string> cdt;
                    std::vector<std::pair<std::string, std::string>> cdts;
                    if (cmd.find("where") + 1) {
                        std::vector<std::string> w_tmp;
                        std::replace(conditions[1].begin(), conditions[1].end(), '(', ' ');
                        std::replace(conditions[1].begin(), conditions[1].end(), ')', ' ');
                        sp.str_process(conditions[1]);
                        sp.str_split(conditions[1], w_tmp, std::regex("\\s?,\\s?"));
                        for (auto i = 0uz; i < w_tmp.size(); ++i) {
                            std::vector<std::string> c_tmp;
                            bool c_find = false;
                            sp.str_split(w_tmp[i], c_tmp, std::regex("\\s?=\\s?"));
                            if (c_tmp[1].front() == '\"') {
                                c_tmp[1].erase(0, 1);
                                c_tmp[1].pop_back();
                            }
                            cdt.insert(std::multimap<std::string, std::string>::value_type(c_tmp[0], c_tmp[1]));
                            cdts.emplace_back(c_tmp[0], c_tmp[1]);
                        }
                    }

                    if(getKeyType(name, res[1]) == 0){ // int
                        table<int> t(name, res[1]);
                        t.openTable();
                        t.updateAll(val, cdts);
                    }
                    else{ // string
                        table<std::string> t(name, res[1]);
                        t.openTable();
                        t.updateAll(val, cdts);
                    }

                    std::multimap<int, std::string> _map;
                    if (f.update_record(name.c_str(), res[1].c_str(), cdt, val)) {
                        std::cout << "Update record successfully!" << std::endl;
                    } else {
                        status &= false;
                    }
                } else {
                    status &= false;
                }
                return status;
            }
        }

        // drop xxx xxx
        {
            // drop database xxx
            std::string drop_database_xxx = "drop\\sdatabase\\s[a-zA-Z]+[a-zA-Z0-9]*";
            std::string drop_database_regex = "^\\s?" + drop_database_xxx + "\\s?" + "$";
            if (std::regex_match(cmd, std::regex(drop_database_regex))) {
                // 正则表达式匹配
                if (f.search_db(res[2].c_str())) {
                    if (name == res[2]) {
                        name = "db";
                    }
                    if (drop_db(res[2].c_str())) {
                        std::cout << "Drop database successfully!" << std::endl;
                    } else {
                        status &= false;
                    }
                } else {
                    status &= false;
                }
                return status;
            }
        }
        {
            // drop table xxx
            std::string drop_table_xxx = "drop\\stable\\s[a-zA-Z]+[a-zA-Z0-9]*";
            std::string drop_table_regex = "^\\s?" + drop_table_xxx + "\\s?" + "$";
            if (std::regex_match(cmd, std::regex(drop_table_regex))) {
                // 正则表达式匹配
                if (f.search_table(name.c_str(), res[2].c_str())) {
                    if (drop_table(name.c_str(), res[2].c_str()) && dropTable(name, res[2])) {
                        std::cout << "Drop table successfully!" << std::endl;
                    } else {
                        status &= false;
                    }
                } else {
                    status &= false;
                }
                return status;
            }
        }

        // error
        std::cout << "Syntax error!" << std::endl;

        return status;
    }
    /*退出时清理*/
    void clear() {
        std::cout << "Bye" << std::endl;
    }

protected:
    /*打印行线*/
    void draw_line(std::vector<int> max_num, int column) {
        for (auto i = 0; i < column; ++i) {
            std::cout << "+-";
            for (auto j = 0; j < max_num[i] + 1; ++j) {
                std::cout << "-";
            }
        }
        std::cout << "+" << std::endl;
    }
    void draw_data(std::vector<int> &max_num, std::vector<std::string> &prop, std::vector<std::vector<std::string>> &data) {
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
    void draw_data(std::vector<int> &max_num, std::vector<column_struct> &prop, std::vector<std::vector<std::string>> &data) {
        std::vector<std::string> tmp_prop;
        for (auto i : prop) {
            tmp_prop.push_back(i.name);
        }
        draw_data(max_num, tmp_prop, data);
    }

public:
    DB() noexcept {
    }
    ~DB() noexcept {
        clear();
    }

    void start() {
        // 主循环
        while (!flag) {
            update();
        }
    }
};