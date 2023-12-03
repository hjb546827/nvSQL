/**
 * @file        DML.cpp
 * @brief       DML
 * @author      hjb
 * @version     1.0
 * @date        2023-11-27
 * @copyright   Copyright (c) 2023
 */

#include "SQL.h"

bool DML::insertRecord(const std::string &database, const std::string &tablename, const std::string &cmd,
                       cache<table> &indexCache, CPUTimer &times) {
    std::vector<std::string> split_res;
    std::vector<tRow> data;
    std::string content = cmd.substr(cmd.find("(") + 1, cmd.find(")") - cmd.find("(") - 1);
    str_process(content);
    str_split(content, split_res, std::regex("\\s?,\\s?"));

    for (auto i : split_res) {
        if (i.front() == ' ')
            i.erase(0, 1);
        tRow d;
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

    if (table<>::getKeyType(database, tablename) == 0) { // int
        int tableID = -1;
        for (auto i = 0; i < (int)indexCache.iCaches.size(); ++i) {
            if (indexCache.iCaches[i].database == database && indexCache.iCaches[i].name == tablename) {
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
            indexCache.iCaches[tableID].init(database, tablename);
            indexCache.iCaches[tableID].openTable();
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<int> &t = indexCache.iCaches[tableID];
        auto pk = t.getPrimaryKey();
        t.insertTable({data[pk].i_value, content});
    } else { // string
        int tableID = -1;
        for (auto i = 0; i < (int)indexCache.sCaches.size(); ++i) {
            if (indexCache.sCaches[i].database == database && indexCache.sCaches[i].name == tablename) {
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
            indexCache.sCaches[tableID].init(database, tablename);
            indexCache.sCaches[tableID].openTable();
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<std::string> &t = indexCache.sCaches[tableID];
        auto pk = t.getPrimaryKey();
        t.insertTable({data[pk].s_value, content});
    }
    times.end();
    return true;
}

bool DML::updateRecord(const std::string &database, const std::string &tablename, const std::string &cmd,
                       cache<table> &indexCache, CPUTimer &times) {
    std::vector<std::string> conditions;
    str_split(cmd, conditions, std::regex("\\swhere\\s"));
    // set
    std::vector<std::string> set_values, values;
    str_split(conditions[0], set_values, std::regex("set\\s?"));
    str_split(set_values[1], values, std::regex("\\s?=\\s?"));
    for (auto &i : values) {
        if (i.front() == '\"') {
            i.erase(0, 1);
            i.pop_back();
        }
    }
    tCdtName_t setCdt{values[0], values[1]};
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
    
    if (table<>::getKeyType(database, tablename) == 0) { // int
        int tableID = -1;
        for (auto i = 0; i < (int)indexCache.iCaches.size(); ++i) {
            if (indexCache.iCaches[i].database == database && indexCache.iCaches[i].name == tablename) {
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
            indexCache.iCaches[tableID].init(database, tablename);
            indexCache.iCaches[tableID].openTable();
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<int> &t = indexCache.iCaches[tableID];
        auto res = t.updateTable(setCdt, cdts);
        times.end();
        return res;
    } else { // string
        int tableID = -1;
        for (auto i = 0; i < (int)indexCache.sCaches.size(); ++i) {
            if (indexCache.sCaches[i].database == database && indexCache.sCaches[i].name == tablename) {
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
            indexCache.sCaches[tableID].init(database, tablename);
            indexCache.sCaches[tableID].openTable();
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<std::string> &t = indexCache.sCaches[tableID];
        auto res = t.updateTable(setCdt, cdts);
        times.end();
        return res;
    }
}

bool DML::deleteRecord(const std::string &database, const std::string &tablename, const std::string &cmd,
                       cache<table> &indexCache, CPUTimer &times) {
    std::vector<std::string> conditions;
    str_split(cmd, conditions, std::regex("\\swhere\\s"));
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

    if (table<>::getKeyType(database, tablename) == 0) { // int
        int tableID = -1;
        for (auto i = 0; i < (int)indexCache.iCaches.size(); ++i) {
            if (indexCache.iCaches[i].database == database && indexCache.iCaches[i].name == tablename) {
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
            indexCache.iCaches[tableID].init(database, tablename);
            indexCache.iCaches[tableID].openTable();
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<int> &t = indexCache.iCaches[tableID];
        auto res = t.eraseTable(cdts);
        times.end();
        return res;
    } else { // string
        int tableID = -1;
        for (auto i = 0; i < (int)indexCache.sCaches.size(); ++i) {
            if (indexCache.sCaches[i].database == database && indexCache.sCaches[i].name == tablename) {
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
            indexCache.sCaches[tableID].init(database, tablename);
            indexCache.sCaches[tableID].openTable();
            indexCache.last = 3 - indexCache.first - indexCache.last;
            indexCache.first = tableID;
        }
        table<std::string> &t = indexCache.sCaches[tableID];
        auto res = t.eraseTable(cdts);
        times.end();
        return res;
    }
}