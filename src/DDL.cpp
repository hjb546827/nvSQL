/**
 * @file        DDL.cpp
 * @brief       DDL
 * @author      hjb
 * @version     1.0
 * @date        2023-11-27
 * @copyright   Copyright (c) 2023
 */

#include "SQL.h"

bool DDL::createDatabase(const std::string &database, CPUTimer& times) {
    std::string dbPath = "data/" + database;
    if (!std::filesystem::create_directory(dbPath)) {
        std::cout << "Failed to create database!" << std::endl;
        return false;
    }
    times.end();
    return true;
}

bool DDL::dropDatabase(const std::string &database, CPUTimer& times) {
    std::string dbPath = "data/" + database;
    try {
        if (!std::filesystem::remove(dbPath)) {
            std::cout << "Failed to drop database!" << std::endl;
            return false;
        }
    } catch (std::filesystem::__cxx11::filesystem_error &e) {
        std::cout << "Completely drop the database? (y/n)" << std::endl;
        std::string rep;
        while (true) {
            if (std::getline(std::cin, rep)) {
                if (rep == "y" || rep == "Y") {
                    if (!std::filesystem::remove_all(dbPath)) {
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
    times.end();
    return true;
}

bool DDL::createTable(const std::string &database, const std::string &tablename, const string &cmd, 
        cache<table>& indexCache, CPUTimer& times) {
    std::vector<std::string> split_res;
    std::vector<tColumn> data;
    std::string content = cmd.substr(cmd.find("(") + 1, cmd.find(")") - cmd.find("(") - 1);
    std::vector<tColumn> primaryKey;
    int pkPos = 0;
    str_process(content);
    str_split(content, split_res, std::regex("\\s?,\\s?"));

    for (auto i = 0uz; i < split_res.size(); ++i) {
        if (split_res[i].front() == ' ')
            split_res[i].erase(0, 1);
        tColumn d;
        std::vector<std::string> s_tmp;
        str_split(split_res[i], s_tmp, " ");
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
        if (d.is_primary) {
            primaryKey.push_back(d);
            pkPos = i;
        }
    }

    if (primaryKey.size() > 1) {
        std::cout << "syntax error!" << endl;
        return false;
    }
    if (primaryKey.size() == 0 && data.size() > 0) {
        primaryKey.push_back(data.front());
        pkPos = 0;
    }
    vector<pair<string, char>> tProps;
    for (auto i : data) {
        tProps.push_back({i.name, 1 - (char)i.type});
    }
    if (primaryKey.front().type == 0) { // int
        int tableID = -1;
        for(auto i = 0; i < (int)indexCache.iCaches.size(); ++i){
            if(indexCache.iCaches[i].database == database && indexCache.iCaches[i].name == tablename){
                if(indexCache.last == i){
                    indexCache.last = 3 - indexCache.last - indexCache.first;
                    indexCache.first = i;
                }
                else{
                    indexCache.first = i;
                }
                tableID = i;
                break;
            }
        }
        if(tableID == -1){
            tableID = indexCache.last;
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<int>& t = indexCache.iCaches[tableID];
        t.renew();
        t.init(database, tablename);
        auto res = t.createTable(tProps, pkPos);
        times.end();
        return res;
    } else { // string
        int tableID = -1;
        for(auto i = 0; i < (int)indexCache.sCaches.size(); ++i){
            if(indexCache.sCaches[i].database == database && indexCache.sCaches[i].name == tablename){
                if(indexCache.last == i){
                    indexCache.last = 3 - indexCache.last - indexCache.first;
                    indexCache.first = i;
                }
                else{
                    indexCache.first = i;
                }
                tableID = i;
                break;
            }
        }
        if(tableID == -1){
            tableID = indexCache.last;
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<std::string>& t = indexCache.sCaches[tableID];
        t.renew();
        t.init(database, tablename);
        auto res = t.createTable(tProps, pkPos);
        times.end();
        return res;
    }
}

bool DDL::dropTable(const std::string &database, const std::string &tablename, CPUTimer& times) {
    auto res = table<>::dropTable(database, tablename);
    times.end();
    return res;
}