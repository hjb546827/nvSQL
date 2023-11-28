/**
 * @file        data_struct.h
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

class table_data {
protected:
    table_data() {}
    table_data(dtype type) {
        this->type = type;
    }
    ~table_data() {}

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
class column_struct : public table_data {
public:
    std::string name = "unknown";
    bool is_primary = false;

    column_struct() noexcept {}
    column_struct(std::string name, dtype type, bool is_primary) noexcept {
        this->name = name;
        this->type = type;
        this->is_primary = is_primary;
    }
    ~column_struct() noexcept {}

    void print_data() {
        std::cout << "data: [" << name << ", " << print_type(type) << ((is_primary == true) ? ", primary" : "") << "]" << std::endl;
    }
};

class row_struct : public table_data {
public:
    std::string s_value = "";
    int i_value = 0;

    row_struct() noexcept {}
    row_struct(std::string s_value) noexcept {
        this->s_value = s_value;
        this->type = STRING;
    }
    row_struct(int i_value) noexcept {
        this->i_value = i_value;
        this->type = INT;
    }
    ~row_struct() noexcept {}

    void print_data() {
        if (this->type == INT)
            std::cout << "data: int[" << i_value << "]" << std::endl;
        else if (this->type == STRING)
            std::cout << "data: string[" << s_value << "]" << std::endl;
        else
            std::cout << "unknown data" << std::endl;
    }

    static bool data_match(std::vector<column_struct> column, std::vector<row_struct> row);
    static bool has_property(std::vector<column_struct> column, std::vector<column_struct> selected);
    static bool has_property(std::vector<column_struct> column, std::string selected);
};

inline bool row_struct::data_match(std::vector<column_struct> column, std::vector<row_struct> row) {
    if (row.size() != column.size())
        return false;
    for (auto i = 0uz; i < row.size(); ++i) {
        if (row[i].type != column[i].type)
            return false;
    }

    return true;
}
inline bool row_struct::has_property(std::vector<column_struct> column, std::vector<column_struct> selected) {
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
inline bool row_struct::has_property(std::vector<column_struct> column, std::string selected) {
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