/**
 * @file        fsystem.h
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
#include <fstream>
#include <cstring>
#include <cstdio>
#include <map>
#include <filesystem>
#include <utility>
#include <vector>
#include "base.h"
#include "data_struct.h"
#include "str_processor.h"

class fsystem {
private:
    std::fstream f;
    str_processor sp;
    const int app = 1;

    /*写入文件*/
    bool fwrite(const char *db, const char *table, std::string data, ioc::w_mode mode = ioc::out) {
        size_t len = std::strlen(data.c_str());

        if (mode == ioc::out)
            f.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary);
        else {
            f.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary | std::ios::app);
            f.write(br, std::strlen(br));
        }
        if (!f.is_open()) {
            std::cout << "Failed to open table!" << std::endl;
            return false;
        }

        f.write(data.c_str(), len);
        f.close();

        return true;
    }
    bool fwrite(const char *db, const char *table, std::vector<int> data, std::vector<std::string> ids, ioc::w_mode mode = ioc::out) {
        if (data.size() != ids.size()) {
            std::cout << "Data mismatch!" << std::endl;
            return false;
        }
        if (mode == ioc::out)
            f.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary);
        else {
            f.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary | std::ios::app);
            f.write(br, std::strlen(br));
        }
        if (!f.is_open()) {
            std::cout << "Failed to open table!" << std::endl;
            return false;
        }

        for (auto i = 0uz; i < data.size(); ++i) {
            std::string tmp;
            if (i == data.size() - 1)
                tmp = ids[i] + " " + std::to_string(data[i]);
            else
                tmp = ids[i] + " " + std::to_string(data[i]) + "\n";
            size_t len = std::strlen(tmp.c_str());
            f.write(tmp.c_str(), len);
        }
        f.close();

        return true;
    }
    bool fwrite(const char *db, const char *table, std::vector<column_struct> data, ioc::w_mode mode = ioc::out) {
        // for creating table
        if (mode == ioc::out)
            f.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary);
        else {
            f.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary | std::ios::app);
            f.write(br, std::strlen(br));
        }
        if (!f.is_open()) {
            std::cout << "Failed to open table!" << std::endl;
            return false;
        }

        for (auto i = 0uz; i < data.size(); ++i) {
            std::string tmp;
            if (i == data.size() - 1)
                tmp = data[i].name + "(" + data[i].get_type() + ")";
            else
                tmp = data[i].name + "(" + data[i].get_type() + ")" + "|";
            size_t len = std::strlen(tmp.c_str());
            f.write(tmp.c_str(), len);
        }
        f.close();

        return true;
    }
    bool fwrite(const char *db, const char *table, std::vector<row_struct> data, ioc::w_mode mode = ioc::out) {
        // for inserting into table
        if (mode == ioc::out)
            f.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary);
        else {
            f.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary | std::ios::app);
            f.write(br, std::strlen(br));
        }
        if (!f.is_open()) {
            std::cout << "Failed to open table!" << std::endl;
            return false;
        }

        for (auto i = 0uz; i < data.size(); ++i) {
            std::string tmp;
            if (i == data.size() - 1)
                tmp = (data[i].type == INT ? std::to_string(data[i].i_value) : data[i].s_value);
            else
                tmp = (data[i].type == INT ? std::to_string(data[i].i_value) : data[i].s_value) + "|";
            size_t len = std::strlen(tmp.c_str());
            f.write(tmp.c_str(), len);
        }
        f.close();

        return true;
    }
    /*读取文件*/
    bool fread(const char *db, const char *table, std::string &res) {
        std::string tmp = "";
        std::ifstream f_read;
        f_read.open(directory_combine(db, table).c_str(), std::ios::in | std::ios::binary);
        if (!f_read.good()) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }

        while (getline(f_read, tmp)) {
            res += tmp + "\n";
        }
        f_read.close();

        return true;
    }
    bool fread(const char *db, const char *table, std::string &res, std::vector<int> &p_order) {
        std::string tmp = "";
        std::ifstream f_read;
        f_read.open(directory_combine(db, table).c_str(), std::ios::in | std::ios::binary);
        if (!f_read.good()) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }

        int cnt = 0;

        while (getline(f_read, tmp) && cnt < 4) {
            cnt++;
        }

        while (getline(f_read, tmp)) {
            for (auto i : p_order) {
                res += get_row_data(tmp, i) + " ";
            }
            res += "\n";
        }

        f_read.close();

        return true;
    }
    bool fread(const char *db, const char *table, std::vector<std::vector<std::string>> &data, int p_num,
               std::vector<int> &max_num, std::vector<int> &p_order, std::multimap<int, std::string> cdt = {}, bool is_and = false) {
        std::string tmp = "";
        std::ifstream f_read;
        f_read.open(directory_combine(db, table).c_str(), std::ios::in | std::ios::binary);
        if (!f_read.good()) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }

        int cnt = 0;

        while (getline(f_read, tmp) && cnt < 4) {
            cnt++;
        }

        while (getline(f_read, tmp)) {
            std::vector<std::string> tmp_data;
            int tmp_max = 0;
            bool is_selected = false;
            std::vector<std::string> sc_tmp;
            for (auto i = 0; i < p_num; ++i) {
                std::string tmp_res = get_row_data(tmp, i);
                sc_tmp.push_back(tmp_res);
            }
            for (auto i = 0uz; i < p_order.size(); ++i) {
                std::string tmp_res = sc_tmp[p_order[i]];
                max_num[i] = tmp_res.size() > max_num[i] ? tmp_res.size() : max_num[i];
                tmp_data.push_back(tmp_res);
                for (auto iter = cdt.find(p_order[i]); iter != cdt.end(); ++iter) {
                    if (tmp_res == iter->second) {
                        is_selected = true;
                    }
                }
            }
            if (is_and) {
                bool a_tmp = true;
                for (auto iter = cdt.begin(); iter != cdt.end(); ++iter) {
                    if (sc_tmp[iter->first] != iter->second) {
                        a_tmp = false;
                    }
                }
                if (a_tmp)
                    data.push_back(tmp_data);
            }
            if (is_selected && !is_and)
                data.push_back(tmp_data);
        }
        f_read.close();

        return true;
    }
    bool fmatch(const char *db, const char *table, std::vector<column_struct> &data) {
        std::ifstream f_read;
        f_read.open(directory_combine(db, table).c_str(), std::ios::in | std::ios::binary);

        if (!f_read.good()) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }

        std::string line_res = "";
        int i = 0;
        try {
            while (getline(f_read, line_res) && i < 2) {
                i++;
            }
            if (line_res.find("## column") + 1) {
                getline(f_read, line_res);

                std::istringstream ss(line_res);
                std::string res;
                while (getline(ss, res, '|')) {
                    std::string content = res.substr(res.find("(") + 1, res.find(")") - res.find("(") - 1);
                    column_struct d;
                    if (content == "int")
                        d.type = INT;
                    else if (content == "string")
                        d.type = STRING;
                    else
                        throw std::exception();

                    data.push_back(d);
                }
            }
        } catch (std::exception e) {
            std::cout << "File format error!" << std::endl;
            return false;
        }
        f_read.close();

        return true;
    }
    bool ftest(const char *db, const char *table) {
        std::ifstream f_read;
        f_read.open(directory_combine(db, table).c_str(), std::ios::in | std::ios::binary);

        if (!f_read.good())
            return false;

        std::string line_res = "";
        int i = 0;
        try {
            while (getline(f_read, line_res) && i < 4) {
                i++;
            }
            if (line_res.find("### row") + 1 == 0) {
                return false;
            }
        } catch (std::exception e) {
            return false;
        }
        f_read.close();

        return true;
    }
    bool get_property(const char *db, const char *table, std::vector<column_struct> &data) {
        std::ifstream f_read;
        f_read.open(directory_combine(db, table).c_str(), std::ios::in | std::ios::binary);

        if (!f_read.good())
            return false;

        std::string line_res = "";
        int i = 0;
        try {
            while (getline(f_read, line_res) && i < 2) {
                i++;
            }
            if (line_res.find("## column") + 1) {
                getline(f_read, line_res);
                std::vector<std::string> properties;
                sp.str_split(line_res, properties, std::regex("\\((int|string)\\)\\|?"));
                for (auto i : properties) {
                    column_struct d;
                    d.name = i;
                    d.type = STRING;
                    data.push_back(d);
                }
                if (data.size() < 1)
                    return false;
            }
        } catch (std::exception e) {
            std::cout << "File format error!" << std::endl;
            return false;
        }
        f_read.close();

        return true;
    }
    /*文件操作*/
    bool _delete(const char *db, const char *table, std::multimap<int, std::string> &record) {
        std::ifstream f_read;
        f_read.open(directory_combine(db, table).c_str(), std::ios::in | std::ios::binary);

        if (!f_read.good()) {
            return false;
        }

        std::vector<std::string> fc;

        std::string line_res = "";
        int i = 0;
        try {
            while (getline(f_read, line_res)) {
                fc.push_back(line_res);
            }
            f_read.close();
            if (fc.size() < 6) {
                throw std::exception();
            }
            std::vector<int> _erase;
            for (auto i = 5uz; i < fc.size(); ++i) {
                bool _skip = false;
                for (auto it = record.begin(); it != record.end(); ++it) {
                    if (get_row_data(fc[i], it->first) != it->second) {
                        _skip = true;
                        break;
                    }
                }
                if (_skip)
                    continue;
                _erase.push_back(i);
            }
            for (auto i : _erase) {
                fc.erase(fc.begin() + i);
            }
            std::ofstream f_write;
            f_write.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary);
            for (auto i = 0uz; i < fc.size(); ++i) {
                f_write.write(fc[i].c_str(), fc[i].size());
                if (i != fc.size() - 1)
                    f_write << '\n';
            }
            f_write.close();

            if (_erase.size() == 0) {
                std::cout << "No record matches!" << std::endl;
                return false;
            }
        } catch (std::exception e) {
            std::cout << "Empty table!" << std::endl;
            return false;
        }

        return true;
    }
    bool _update(const char *db, const char *table, int p_num, std::multimap<int, std::string> &record, std::pair<int, std::string> &data) {
        std::ifstream f_read;
        f_read.open(directory_combine(db, table).c_str(), std::ios::in | std::ios::binary);

        if (!f_read.good()) {
            return false;
        }

        std::vector<std::string> fc;

        std::string line_res = "";
        int i = 0;
        try {
            while (getline(f_read, line_res)) {
                fc.push_back(line_res);
            }
            f_read.close();
            if (fc.size() < 6) {
                throw std::exception();
            }
            std::vector<int> _update;
            // find matching record
            for (auto i = 5uz; i < fc.size(); ++i) {
                bool _skip = false;
                for (auto it = record.begin(); it != record.end(); ++it) {
                    if (get_row_data(fc[i], it->first) != it->second) {
                        _skip = true;
                        break;
                    }
                }
                if (_skip)
                    continue;
                _update.push_back(i);
            }
            for (auto i : _update) {
                std::vector<std::string> row_data;
                for (auto j = 0; j < p_num; ++j) {
                    std::string tmp_res = get_row_data(fc[i], j);
                    if (data.first == j) {
                        tmp_res = data.second;
                    }
                    row_data.push_back(tmp_res);
                }
                std::string tmp;
                for (auto j = 0uz; j < row_data.size(); ++j) {
                    if (j == p_num - 1)
                        tmp += row_data[j];
                    else
                        tmp += row_data[j] + "|";
                }
                fc[i] = tmp;
            }
            std::ofstream f_write;
            f_write.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary);
            for (auto i = 0uz; i < fc.size(); ++i) {
                f_write.write(fc[i].c_str(), fc[i].size());
                if (i != fc.size() - 1)
                    f_write << '\n';
            }
            f_write.close();

            if (_update.size() == 0) {
                std::cout << "No record matches!" << std::endl;
                return false;
            }
        } catch (std::exception e) {
            std::cout << "Empty table!" << std::endl;
            return false;
        }

        return true;
    }

protected:
    std::string directory_create(const char *d) {
        std::string s(d);

        return "data/" + s;
    }
    std::string directory_combine(const char *c1, const char *c2) {
        std::string s1(c1);
        std::string s2(c2);

        return "data/" + s1 + "/" + s2 + ".data";
    }
    std::string get_row_data(std::string row, int pos) {
        std::vector<std::string> res;
        sp.str_split(row, res, "|");
        if (pos >= 0 && pos < res.size())
            return res[pos];
        else {
            std::cout << "Error: out of range!" << std::endl;
            return "";
        }
    }

public:
    fsystem() noexcept {}
    fsystem(const char *str, const char *db) noexcept {
        size_t len = std::strlen(str) + 1;
        std::cout << len << std::endl;
        f.open(db, std::ios::out | std::ios::binary | std::ios::app);

        f.write(str, len);
        f.close();

        f.open(db, std::ios::in | std::ios::binary);

        char s[len];
        while (f.read(s, len)) {
            int readb = f.gcount();
            std::cout << s << std::endl;
        }
        f.close();
    }
    ~fsystem() noexcept {}
    // --------------------------database--------------------------
    /*创建数据库*/
    bool create_db(const char *db) {
        // f.open(db, std::ios::out | std::ios::binary);
        // f.close();

        // if(!search_db(db)){
        //     std::cout << "Failed to create database!" << std::endl;
        //     return false;
        // }

        // return true;
        if (!std::filesystem::create_directory(directory_create(db))) {
            std::cout << "Failed to create database!" << std::endl;
            return false;
        }

        return true;
    }
    /*删除数据库*/
    bool drop_db(const char *db) {
        // if(!remove(db))
        //     return true;
        // else
        //     return false;
        try {
            if (!std::filesystem::remove(directory_create(db))) {
                std::cout << "Failed to drop database!" << std::endl;
                return false;
            }
        } catch (std::filesystem::__cxx11::filesystem_error e) {
            std::cout << "Completely drop the database? (y/n)" << std::endl;
            std::string rep;
            while (true) {
                if (std::getline(std::cin, rep)) {
                    if (rep == "y" || rep == "Y") {
                        if (!std::filesystem::remove_all(directory_create(db))) {
                            std::cout << "Failed to drop database!" << std::endl;
                            return false;
                        }
                        break;
                    } else if (rep == "n" || rep == "N") {
                        break;
                    } else {
                        continue;
                    }
                }
            }
        }

        return true;
    }
    /*检测数据库*/
    bool search_db(const char *db) {
        // std::ifstream f_test(db);
        // if(!f_test.good()){
        //     std::cout << "Database not exists!" << std::endl;
        //     return false;
        // }
        // else
        //     return true;
        if (!std::filesystem::exists(directory_create(db))) {
            std::cout << "Database not exists!" << std::endl;
            return false;
        } else
            return true;
    }
    /*清空数据库*/
    bool fresh_db(const char *db) {
        // std::fstream f_fresh(db, std::ios::out);

        return true;
    }

    // ---------------------------table----------------------------
    /*创建表*/
    bool create_table(const char *db, const char *table, const std::vector<int> &ints, const std::vector<std::string> &ids) {
        f.open(directory_combine(db, table).c_str(), std::ios::out | std::ios::binary);
        f.close();

        if (!search_table(db, table)) {
            std::cout << "Failed to create table!" << std::endl;
            return false;
        }
        std::string t(table);
        if (!fwrite(db, table, "# table\n" + t))
            return false;
        if (!fwrite(db, table, "## column", ioc::app))
            return false;
        if (fwrite(db, table, ints, ids, ioc::app))
            return false;

        return true;
    }
    bool create_table(const char *db, const char *table, const std::vector<column_struct> &data) {
        std::string filename = directory_combine(db, table);
        if(std::filesystem::exists(filename)){
            std::cout << "table already exists!" << std::endl;
            return false;
        }
        f.open(filename.c_str(), std::ios::out | std::ios::binary);
        f.close();

        if (!search_table(db, table)) {
            std::cout << "Failed to create table!" << std::endl;
            return false;
        }
        std::string t(table);
        if (!fwrite(db, table, "# table\n" + t))
            return false;
        if (!fwrite(db, table, "## column", ioc::app))
            return false;
        if (!fwrite(db, table, data, ioc::app))
            return false;

        return true;
    }
    /*删除表*/
    bool drop_table(const char *db, const char *table) {
        if (!remove(directory_combine(db, table).c_str()))
            return true;
        else
            return false;
        return true;
    }
    /*插入表*/
    bool insert_table(const char *db, const char *table, std::vector<row_struct> &data) {
        std::vector<column_struct> c_data;
        if (!fmatch(db, table, c_data))
            return false;
        if (!row_struct::data_match(c_data, data)) {
            std::cout << "Data mismatch!" << std::endl;
            return false;
        }
        if (!ftest(db, table))
            if (!fwrite(db, table, "### row", ioc::app))
                return false;
        if (!fwrite(db, table, data, ioc::app))
            return false;
        return true;
    }
    /*检测表*/
    bool search_table(const char *db, const char *table) {
        std::ifstream f_test(directory_combine(db, table).c_str());
        if (!f_test.good()) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        } else
            return true;
    }
    /*查询表*/
    bool query_table(const char *db, const char *table, std::string &data) {
        return fread(db, table, data);
    }
    bool select_table(const char *db, const char *table, std::vector<column_struct> &data, std::string &res) {
        std::vector<column_struct> property;
        if (!get_property(db, table, property)) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }
        if (!row_struct::has_property(property, data)) {
            std::cout << "Unknown column!" << std::endl;
            return false;
        }
        std::vector<int> selected;
        for (auto i : data) {
            for (auto j = 0uz; j < property.size(); ++j) {
                if (i.name == property[j].name)
                    selected.push_back(j);
            }
        }
        if (!fread(db, table, res, selected)) {
            return false;
        }

        return true;
    }
    // bool select_table(const char* db, const char* table, std::vector<column_struct>& data,
    //          std::vector<std::vector<std::string>>& res, std::vector<int>& max_num){
    //     std::vector<column_struct> property;
    //     if(!get_property(db, table, property)){
    //         std::cout << "Table not exists!" << std::endl;
    //         return false;
    //     }
    //     if(data.size() == 0uz){
    //         for(auto i : property)
    //             data.push_back(i);
    //     }
    //     if(!row_struct::has_property(property, data)){
    //         std::cout << "Unknown column!" << std::endl;
    //         return false;
    //     }
    //     std::vector<int> selected;
    //     for(auto i : data){
    //         for(auto j = 0uz; j < property.size(); ++j){
    //             if(i.name == property[j].name)
    //                 selected.push_back(j);
    //         }
    //     }
    //     for(auto i = 0uz; i < data.size(); ++i){
    //         int tmp_num = data[i].name.size();
    //         max_num.push_back(tmp_num);
    //     }
    //     if(!fread(db, table, res, max_num, selected)){
    //         return false;
    //     }

    //     return true;
    // }
    bool select_table(const char *db, const char *table, std::vector<column_struct> &data,
                      std::vector<std::vector<std::string>> &res, std::vector<int> &max_num, std::multimap<std::string, std::string> &cdt) {
        std::vector<column_struct> property;
        if (!get_property(db, table, property)) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }
        if (data.size() == 0uz) {
            for (auto i : property)
                data.push_back(i);
        }
        if (!row_struct::has_property(property, data)) {
            std::cout << "Unknown column!" << std::endl;
            return false;
        }
        for (auto &i : cdt) {
            if (!row_struct::has_property(property, i.first)) {
                std::cout << std::format("Unknown column {}!\n", i.first);
                return false;
            }
        }
        std::vector<int> selected;
        std::multimap<int, std::string> needed;
        for (auto i : data) {
            for (auto j = 0uz; j < property.size(); ++j) {
                if (i.name == property[j].name)
                    selected.push_back(j);
            }
        }
        for (auto i : cdt) {
            for (auto j = 0uz; j < property.size(); ++j) {
                if (i.first == property[j].name)
                    needed.insert(std::multimap<int, std::string>::value_type(j, i.second));
            }
        }
        for (auto i = 0uz; i < data.size(); ++i) {
            int tmp_num = data[i].name.size();
            max_num.push_back(tmp_num);
        }
        if (!fread(db, table, res, property.size(), max_num, selected, needed, true)) {
            return false;
        }

        return true;
    }
    /*删除记录*/
    bool delete_record(const char *db, const char *table, std::multimap<std::string, std::string> &cdt) {
        // std::multimap<int, std::string> _map{{0, "1"}, {1, "w"}, {2, "98"}};
        std::vector<column_struct> property;
        if (!get_property(db, table, property)) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }
        for (auto &i : cdt) {
            if (!row_struct::has_property(property, i.first)) {
                std::cout << std::format("Unknown column {}!\n", i.first);
                return false;
            }
        }
        std::multimap<int, std::string> needed;
        for (auto i : cdt) {
            for (auto j = 0uz; j < property.size(); ++j) {
                if (i.first == property[j].name)
                    needed.insert(std::multimap<int, std::string>::value_type(j, i.second));
            }
        }
        if (!_delete(db, table, needed))
            return false;
        return true;
    }
    /*更新记录*/
    bool update_record(const char *db, const char *table, std::multimap<std::string, std::string> &cdt, std::pair<std::string, std::string> &data) {
        // std::multimap<int, std::string> _map{{0, "1"}, {1, "w"}, {2, "98"}};
        std::vector<column_struct> property;
        if (!get_property(db, table, property)) {
            std::cout << "Table not exists!" << std::endl;
            return false;
        }
        for (auto &i : cdt) {
            if (!row_struct::has_property(property, i.first)) {
                std::cout << std::format("Unknown column {}!\n", i.first);
                return false;
            }
        }
        std::multimap<int, std::string> needed;
        std::pair<int, std::string> _data{0, data.second};
        for (auto i = 0uz; i < property.size(); ++i) {
            if (data.first == property[i].name)
                _data.first = i;
        }
        for (auto i : cdt) {
            for (auto j = 0uz; j < property.size(); ++j) {
                if (i.first == property[j].name)
                    needed.insert(std::multimap<int, std::string>::value_type(j, i.second));
            }
        }
        if (!_update(db, table, property.size(), needed, _data))
            return false;
        return true;
    }
};