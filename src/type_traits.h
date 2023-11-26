/**
 * @file		type_traits.h
 * @brief	    
 * @author		hjb
 * @version		1.0
 * @date		2023-11-12
 * @copyright	Copyright (c) 2023
 */

#include <iostream>

template <typename T>
struct toStr{
    std::string s;
    toStr(T v){
        this->s = std::to_string(v);
    }
    std::string operator()(){
        return s;
    }
};
template <>
struct toStr<std::string>{
    std::string s;
    toStr(std::string v){
        this->s = v;
    }
    std::string operator()(){
        return s;
    }
};

template <typename T>
struct keyFormatConverter{
    T v;
    keyFormatConverter(T v){
        this->v = v;
    }
    T operator()(){
        return v;
    }
};
template <>
struct keyFormatConverter<int>{
    int v;
    keyFormatConverter(std::string s){
        this->v = std::atoi(s.c_str());
    }
    int operator()(){
        return v;
    }
};

template <typename T>
struct getKeySize{
    int n;
    getKeySize(T v){
        this->n = v.size();
    }
    int operator()(){
        return n;
    }
};
template <>
struct getKeySize<int>{
    int n;
    getKeySize(int v){
        this->n = 4;
    }
    int operator()(){
        return n;
    }
};

template <typename T>
struct getKeyData{
    T key;
    getKeyData(T v){
        this->key = v;
    }
    char* operator()(){
        return key.data();
    }
};
template <>
struct getKeyData<int>{
    int key;
    getKeyData(int v){
        this->key = v;
    }
    char* operator()(){
        return (char*)&key;
    }
};

template <typename T, typename Q>
struct setIFirst{
    setIFirst(T& ifst, Q& ifst2){
        ;
    }
};
template <typename T>
struct setIFirst<T, T>{
    setIFirst(T& ifst, T& ifst2){
        ifst = ifst2;
    }
};

template <typename T>
constexpr inline bool keyTypeIsString = true;
template <>
constexpr inline bool keyTypeIsString<int> = false;