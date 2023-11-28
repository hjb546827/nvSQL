/**
 * @file        type_traits.h
 * @brief
 * @author      hjb
 * @version     1.0
 * @date        2023-11-12
 * @copyright   Copyright (c) 2023
 */

#include <iostream>

/**
 * @brief   int/string转换为string
 * @tparam  T int/string
 */
template <typename T>
struct toStr {
    std::string s;
    toStr(T v) {
        this->s = std::to_string(v);
    }
    std::string operator()() {
        return s;
    }
};
template <>
struct toStr<std::string> {
    std::string s;
    toStr(std::string v) {
        this->s = v;
    }
    std::string operator()() {
        return s;
    }
};

/**
 * @brief   主键转换为对应格式
 * @tparam  T int/string
 */
template <typename T>
struct keyFormatConverter {
    T v;
    keyFormatConverter(T v) {
        this->v = v;
    }
    T operator()() {
        return v;
    }
};
template <>
struct keyFormatConverter<int> {
    int v;
    keyFormatConverter(std::string s) {
        this->v = std::atoi(s.c_str());
    }
    int operator()() {
        return v;
    }
};

/**
 * @brief   主键在磁盘中占用空间大小
 * @tparam  T int/string
 */
template <typename T>
struct getKeySize {
    int n;
    getKeySize(T v) {
        this->n = v.size();
    }
    int operator()() {
        return n;
    }
};
template <>
struct getKeySize<int> {
    int n;
    getKeySize(int v) {
        this->n = 4;
    }
    int operator()() {
        return n;
    }
};

/**
 * @brief   主键数据转换为char*格式
 * @tparam  T int/string
 */
template <typename T>
struct getKeyData {
    T key;
    getKeyData(T v) {
        this->key = v;
    }
    char *operator()() {
        return key.data();
    }
};
template <>
struct getKeyData<int> {
    int key;
    getKeyData(int v) {
        this->key = v;
    }
    char *operator()() {
        return (char *)&key;
    }
};

/**
 * @brief   将Q类型值赋值给T类型值，Q、T必须相同（欺骗编译器以通过检查）
 * @tparam  T int/string
 * @tparam  Q int/string
 */
template <typename T, typename Q>
struct setIVal {
    setIVal(T &ifst, Q &ifst2) {}
};
template <typename T>
struct setIVal<T, T> {
    setIVal(T &ifst, T &ifst2) {
        ifst = ifst2;
    }
};

/**
 * @brief   判断T是否为string类型
 * @tparam  T int/string
 */
template <typename T>
constexpr inline bool keyTypeIsString = true;
template <>
constexpr inline bool keyTypeIsString<int> = false;