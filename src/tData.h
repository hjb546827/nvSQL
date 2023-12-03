/**
 * @file        tData.h
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
#include <vector>

enum dtype {
    INT,
    STRING
};

class tData {
protected:
    tData() {}
    tData(dtype type) {
        this->type = type;
    }
    ~tData() {}

    virtual std::string print_type(dtype type) {
        if (type == INT)
            return "int";
        if (type == STRING)
            return "string";

        return "";
    }

public:
    dtype type = (dtype) - 1;

    virtual void print_data() = 0;

    virtual std::string get_type() {
        return print_type(type);
    }
};
class tColumn : public tData {
public:
    std::string name = "unknown";
    bool is_primary = false;

    tColumn() noexcept {}
    tColumn(std::string name, dtype type, bool is_primary) noexcept {
        this->name = name;
        this->type = type;
        this->is_primary = is_primary;
    }
    ~tColumn() noexcept {}

    void print_data() {
        std::cout << "data: [" << name << ", " << print_type(type) << ((is_primary == true) ? ", primary" : "") << "]" << std::endl;
    }
};

class tRow : public tData {
public:
    std::string s_value = "";
    int i_value = 0;

    tRow() noexcept {}
    tRow(std::string s_value) noexcept {
        this->s_value = s_value;
        this->type = STRING;
    }
    tRow(int i_value) noexcept {
        this->i_value = i_value;
        this->type = INT;
    }
    ~tRow() noexcept {}

    void print_data() {
        if (this->type == INT)
            std::cout << "data: int[" << i_value << "]" << std::endl;
        else if (this->type == STRING)
            std::cout << "data: string[" << s_value << "]" << std::endl;
        else
            std::cout << "unknown data" << std::endl;
    }

    static bool data_match(std::vector<tColumn> column, std::vector<tRow> row);
    static bool has_property(std::vector<tColumn> column, std::vector<tColumn> selected);
    static bool has_property(std::vector<tColumn> column, std::string selected);
};

inline bool tRow::data_match(std::vector<tColumn> column, std::vector<tRow> row) {
    if (row.size() != column.size())
        return false;
    for (auto i = 0uz; i < row.size(); ++i) {
        if (row[i].type != column[i].type)
            return false;
    }

    return true;
}
inline bool tRow::has_property(std::vector<tColumn> column, std::vector<tColumn> selected) {
    bool tmp = false;
    if (column.size() < selected.size())
        return false;
    for (auto i = 0uz; i < selected.size(); ++i) {
        for (auto j : column) {
            if (selected[i].name == j.name) {
                tmp = true;
                break;
            }
        }
        if (tmp == false)
            return false;
        tmp = false;
    }
    return true;
}
inline bool tRow::has_property(std::vector<tColumn> column, std::string selected) {
    bool tmp = false;
    if (column.size() < 1uz)
        return false;
    for (auto j : column) {
        if (selected == j.name) {
            tmp = true;
            break;
        }
    }

    return tmp;
}