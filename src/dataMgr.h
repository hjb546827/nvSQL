//dataMgr.h

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <regex>
#include <vector>
#include <string>
#include <initializer_list>
#include <format>
#include <fstream>
#include <cstdio>

using namespace std;

static const string dataPos = "data/";
static const size_t maxRecSize = 256 - 1;
static const size_t maxPageSize = 8 * 1024;
static const size_t maxPropSize = 1024 - 4;
static const size_t bpTreeLevel = 3;

struct record{
/*
 * table.dat    数据文件
 * record:
 * # record头
 * 00000000 00000000 00000000 00000000
 *   byte1    byte2    byte3    byte4
 * byte1：记录R占有的record总数，首条为(sizeof(R)-4)/maxRecSize+1，不为首条均为0
 * byte2：该record长度的第一个字节
 * byte3：该record长度的第二个字节
 * byte4：下一个record位置，已是末尾则为0
 * # record数据
 * xxxxxx
*/
/**
 * table.ind  索引文件
 * line1：b+Tree层序遍历序列
 * line2：bpTree结构中indexs数组
 * line3：bpTree结构中叶子节点数据
*/
/**
 * table.prof   配置文件
 * # 前4个字节
 * xxxx 主键类型int/string
 * # 次4个字节
 * xxxx 主键位置pos
 * # 次4个字节
 * xxxx 表属性个数n
 * # 次n个字节
 * x    对应属性是否为int类型
 * # 后4个字节
 * xxxx dat文件写指针位置
*/
public:
    using value_type = char;
    value_type data[maxRecSize];
    uint16_t size = 0;
    
    record(){
        memset(data, 0, maxRecSize);
        size = 4;
    }
    record(string s, bool _endr = true) : record(){
        if(_endr){
            data[0] = 1;
        }
        else{
            data[0] = 0;
        }
        push_back(s);
    }
    record(int v, bool _endr = true) : record(){
        if(_endr){
            data[0] = 1;
        }
        else{
            data[0] = 0;
        }
        push_back(v);
    }

    void push_back(string s){
        int _size = s.size();
        convert(_size);
        convert(s);
    }

    void push_back(int v){
        int _size = 4;
        convert(_size);
        convert(v);
    }

    void save(fstream& f){
        int _size = size - 4;
        memcpy(data + 1, &_size, 2);
        f.write(data, maxRecSize);
    }

    size_t fill(vector<string>& v){
        auto n = v.size();
        for(auto i = 0uz; i < n; ++i){
            if(v[i].front() == 'i'){
                if(size + 4 + 4 >= (int)maxRecSize){
                    return i;
                }
                v[i].erase(0, 1);
                int iData = atoi(v[i].c_str());
                push_back(iData);
            }
            else{
                if(size + v[i].size() + 4 > (int)maxRecSize){
                    return i;
                }
                v[i].erase(0, 1);
                string sData = v[i];
                push_back(sData);
            }
        }
        return n;
    }

    void make_head(int v){
        memcpy(data, &v, 4);
    }

    friend ostream& operator<<(ostream& os, record& r){
        os << r.data << endl;
        return os;
    }
    
private:
    void convert(string& s){
        memcpy(data + size, s.data(), s.size());
        size += s.size();
    }
    void convert(int& v){
        memcpy(data + size, &v, 4);
        size += 4;
    }
};

static inline void str_split(string str, vector<string> &v, const string split)
{
    char *s = new char[str.size() + 1];
    strcpy(s, str.c_str());
    char *p = strtok(s, split.c_str());

    while (p)
    {
        v.push_back(p);
        p = strtok(NULL, split.c_str());
    }

    delete[] s;
}
static inline void str_split(std::string str, std::vector<std::string> &v, std::regex reg)
{
    std::regex_token_iterator p(str.begin(), str.end(), reg, -1);
    decltype(p) end;
    for (; p != end; ++p)
    {
        v.push_back(p->str());
    }
}
static inline void str_split(string str, vector<string> &v, const char split)
{
    str_split(str, v, string(1, split));
}

class dataMgr {
private:
    string filename = dataPos + "1/" + "1.dat";
    string database = "1";
    string table = "1";
    int keyType = 1; // string : 0; int : 1; 

protected:

public:
    dataMgr() = default;
    dataMgr(int key_type) : dataMgr() {
        this->keyType = key_type;
    };
    dataMgr(string database, string table, int key_type) noexcept {
        this->keyType = key_type;
        init(database, table);
    }
    ~dataMgr() noexcept {}

    void setKeyType(int key_type){
        this->keyType = key_type;
    }

    void init(string database, string table){
        this->filename = dataPos + database + "/" + table + ".dat";
        this->database = database;
        this->table = table;
    }

    bool createRecord(string& s){
        vector<record> recs;
        makeRecords(this->database, this->table, recs, s);
        makeTable(this->database, this->table, recs);

        return true;
    }

    void readRecord(string& s, int pos){
        if(!filesystem::exists(this->filename)){
            cout << "empty table!" << endl;
            return;
        }
        else{
            ifstream file(this->filename, ios::in | ios::binary);
            int offset = pos * (maxRecSize + 1);
            file.seekg(offset + 1, ios::beg);
            uint16_t recSize = 0;
            file.read((char*)&recSize, sizeof(recSize));
            char _res[recSize];
            file.seekg(1, ios::cur);
            file.read(_res, sizeof(_res));
            s = string(_res, sizeof(_res));
            file.close();
        }
    }
    
    void updateRecord(int pos, string s){
        vector<record> recs;
        makeRecords(this->database, this->table, recs, s);
        if(recs.size() > 1){
            cout << "out of bounds!" << endl;
            return;
        }
        if(!filesystem::exists(this->filename)){
            cout << "empty table!" << endl;
            return;
        }
        else{
            fstream file(this->filename, ios::in | ios::out | ios::binary);

            int offset = pos * (maxRecSize + 1);
            file.seekp(offset, ios::beg);
            recs[0].save(file);
            file.seekp(ios::beg);
            file.close();
        }
    }

    bool deleteRecord(int pos){
        fstream _fi(this->filename, ios::in | ios::out | ios::binary);
        int offset = pos * (maxRecSize + 1);
        _fi.seekp(offset, ios::beg);
        char blank[maxRecSize];
        memset(blank, 0, maxRecSize);
        _fi.write(blank, maxRecSize);
        _fi.seekp(ios::beg);
        _fi.close();

        return true;
    }

    void profInit(){
        string prof = dataPos + database + "/" + table + ".prof";
        if(!filesystem::exists(filename)){
            fstream file(filename, ios::binary | ios::out);
            file.seekp(maxPageSize - 1);
            file.write("", 1);
            
            fstream _file(prof, ios::binary | ios::out | ios::in);
            _file.seekp(maxPropSize, ios::beg);
            int fptr = 0;
            _file.write((char*)&fptr, 4);
            _file.seekp(ios::beg);
            _file.close();
            file.seekp(ios::beg);
            file.close();
        }
    }

private:
    void makeRecords(string const database, string const tablename, vector<record>& recs, string s){
        vector<string> _split;
        str_split(s, _split, regex("((\"\\s*,\\s*\")|(\"\\s*,\\s*)|(\\s*,\\s*\")|(\\s*,\\s*))"));
        string filename = dataPos + database + "/" + tablename + ".prof";
        ifstream file(filename, ios::in | ios::binary);
        file.seekg(8, ios::beg);
        char _r[4] = {};
        file.read(_r, 4);
        int fieldNums = 0;
        memcpy(&fieldNums, _r, 4);
        if(fieldNums != (int)_split.size()){
            cout << "data mismatch!" << endl;
            file.close();
            return;
        }
        for(auto& i : _split){
            char _signal = 0;
            file.read(&_signal, 1);
            char _type = (_signal == 0 ? 's' : 'i');
            i.insert(i.begin(), _type);
        }
        file.close();
        if(_split.front()[1] == '\"')
            _split.front().erase(1, 1);
        if(_split.back().back() == '\"')
            _split.back().pop_back();

        auto beg = 0uz;
        int cnts = 0;
        while(beg < _split.size()){
            record rec;
            beg = rec.fill(_split);
            recs.push_back(rec);
            cnts++;
            if(beg < _split.size()){
                cout << "data out of bounds!" << endl;
                remove((dataPos + database + "/" + tablename + ".dat").c_str());
                remove((dataPos + database + "/" + tablename + ".ind").c_str());
                remove((dataPos + database + "/" + tablename + ".prof").c_str());
                exit(1);
            }
        }
        recs.front().make_head(cnts);
    }

    void makeTable(string const database, string const tablename, vector<record>& recs){
        string filename = dataPos + database + "/" + tablename + ".dat";
        if(!filesystem::exists(filename)){
            fstream file(filename, ios::binary | ios::out);
            int cnts = 0;
            int fptr = 0;
            for(auto i = 0uz; i < recs.size(); ++i){
                if(recs.size() > 1 && i != recs.size() - 1){
                    recs[i].data[3] = cnts + 1;
                }
                recs[i].save(file);
                cnts++;
                fptr = file.tellp();
                file.seekp(maxPageSize * cnts - 1);
                file.write("", 1);
            }
            fstream _file(dataPos + database + "/" + tablename + ".prof", ios::binary | ios::out | ios::in);
            _file.seekp(maxPropSize, ios::beg);
            _file.write((char*)&fptr, 4);
            _file.seekp(ios::beg);
            _file.close();
            file.seekp(ios::beg);
            file.close();
        }
        else{
            fstream file(filename, ios::in | ios::out | ios::binary);
            fstream _file(dataPos + database + "/" + tablename + ".prof", ios::in | ios::out | ios::binary);
            int rptr = 0;
            char _r[4] = {};
            _file.seekg(maxPropSize, ios::beg);
            _file.read(_r, 4);
            _file.seekg(ios::beg);
            memcpy(&rptr, _r, 4);
            int cnts = rptr / maxPageSize;
            rptr = rptr == 0 ? -1 : rptr;
            int fptr = 0;
            file.seekp(rptr + 1, ios::beg);
            for(auto i = 0uz; i < recs.size(); ++i){
                if(recs.size() > 1 && i != recs.size() - 1){
                    recs[i].data[3] = cnts + 1;
                }
                recs[i].save(file);
                cnts++;
                fptr = file.tellp();
                file.seekp(maxPageSize * cnts - 1);
                file.write("", 1);
            }
            _file.seekp(maxPropSize, ios::beg);
            _file.write((char*)&fptr, 4);
            _file.seekp(ios::beg);
            _file.close();
            file.seekp(ios::beg);
            file.close();
        }
    }
};