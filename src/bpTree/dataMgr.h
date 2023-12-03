/**
 * @file        dataMgr.h
 * @brief       磁盘数据写入及读取
 *              table.dat    数据文件
 *                  record:
 *                  # record头
 *                  00000000 00000000 00000000 00000000
 *                    byte1    byte2    byte3    byte4
 *                  byte1：记录R占有的record总数，首条为(sizeof(R)-4)/maxRecSize+1，不为首条均为0
 *                  byte2：该record长度的第一个字节
 *                  byte3：该record长度的第二个字节
 *                  byte4：下一个record位置，已是末尾则为0
 *                  # record数据
 *                  xxxxxx
 *              table.ind  索引文件
 *                  line1：b+Tree层序遍历序列
 *                  line2：bpTree结构中indexs数组
 *                  line3：bpTree结构中叶子节点数据
 *              table.prof   配置文件
 *                  # 前4个字节
 *                  xxxx 主键类型int/string
 *                  # 次4个字节
 *                  xxxx 主键位置pos
 *                  # 次4个字节
 *                  xxxx 表属性个数n
 *                  # 次n个字节
 *                  x    对应属性是否为int类型
 *                  # 后4个字节
 *                  xxxx dat文件写指针位置
 * @author      hjb
 * @version     1.0
 * @date        2023-11-21
 * @copyright   Copyright (c) 2023
 */

#pragma once

#include <cstring>
#include <vector>
#include <string>
#include <fstream>
#include "../utility.h"

namespace bpT {
using namespace std;

static const string dataPos = "data/";
static const size_t maxRecSize = 256 - 1;
static const size_t maxPageSize = 8 * 1024;
static const size_t maxPropSize = 1024 - 4;
static const size_t bpTreeLevel = 3;

/**
* @brief   记录
*/
struct record {
public:
    using value_type = char;
    value_type data[maxRecSize];
    uint16_t size = 0;

    record() {
        memset(data, 0, maxRecSize);
        size = 4;
    }
    record(string s, bool _endr = true) : record() {
        if (_endr) {
            data[0] = 1;
        } else {
            data[0] = 0;
        }
        push_back(s);
    }
    record(int v, bool _endr = true) : record() {
        if (_endr) {
            data[0] = 1;
        } else {
            data[0] = 0;
        }
        push_back(v);
    }

    /**
    * @brief   插入string数据
    * @param   s
    */
    void push_back(string s) {
        int _size = s.size();
        convert(_size);
        convert(s);
    }
    /**
    * @brief   插入int数据
    * @param   v
    */
    void push_back(int v) {
        int _size = 4;
        convert(_size);
        convert(v);
    }

    /**
    * @brief   存储到磁盘
    * @param   f
    */
    void save(fstream &f) {
        int _size = size - 4;
        memcpy(data + 1, &_size, 2);
        f.write(data, maxRecSize);
    }

    /**
    * @brief   数据装入record结构体
    * @param   v
    * @return  size_t record剩余空间
    */
    size_t fill(vector<string> &v) {
        auto n = v.size();
        for (auto i = 0uz; i < n; ++i) {
            if (v[i].front() == 'i') {
                if (size + 4 + 4 >= (int)maxRecSize) {
                    return i;
                }
                v[i].erase(0, 1);
                int iData = atoi(v[i].c_str());
                push_back(iData);
            } else {
                if (size + v[i].size() + 4 > (int)maxRecSize) {
                    return i;
                }
                v[i].erase(0, 1);
                string sData = v[i];
                push_back(sData);
            }
        }
        return n;
    }

    /**
    * @brief   写入文件头
    * @param   v
    */
    void make_head(int v) {
        memcpy(data, &v, 4);
    }

    /**
    * @brief   打印record中数据
    * @param   os
    * @param   r
    * @return  ostream&
    */
    friend ostream &operator<<(ostream &os, record &r) {
        os << r.data << endl;
        return os;
    }

private:
    /**
    * @brief   string类型的数据转为char*
    * @param   s
    */
    void convert(string &s) {
        memcpy(data + size, s.data(), s.size());
        size += s.size();
    }
    /**
    * @brief   int类型的数据转为char*
    * @param   v
    */
    void convert(int &v) {
        memcpy(data + size, &v, 4);
        size += 4;
    }
};

/**
* @brief   bpTree对象的辅助结构体，负责数据在磁盘上的写入与读取
*/
class dataMgr {
private:
    string filename = dataPos + "db/" + "table.dat";    // 表的dat文件路径
    string database = "db";     // 数据库名
    string table = "table";     // 表名
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

    /**
    * @brief   设置表的主键类型（int/string）
    * @param   key_type    0/1
    */
    void setKeyType(int key_type) {
        this->keyType = key_type;
    }

    /**
    * @brief   初始化表存储
    * @param   database
    * @param   table
    */
    void init(string database, string table) {
        this->filename = dataPos + database + "/" + table + ".dat";
        this->database = database;
        this->table = table;
    }

    /**
    * @brief   创建表记录并存储
    * @param   s
    * @return  true
    * @return  false
    */
    bool createRecord(string &s) {
        vector<record> recs;
        makeRecords(this->database, this->table, recs, s);
        makeTable(this->database, this->table, recs);

        return true;
    }

    /**
    * @brief   在磁盘上读取表记录
    * @param   s
    * @param   pos
    */
    void readRecord(string &s, int pos) {
        if (!filesystem::exists(this->filename)) {
            cout << "empty table!" << endl;
            return;
        } else {
            ifstream file(this->filename, ios::in | ios::binary);
            int offset = pos * (maxRecSize + 1);
            file.seekg(offset + 1, ios::beg);
            uint16_t recSize = 0;
            file.read((char *)&recSize, sizeof(recSize));
            char _res[recSize];
            file.seekg(1, ios::cur);
            file.read(_res, sizeof(_res));
            s = string(_res, sizeof(_res));
            file.close();
        }
    }
    void readRecord(vector<string> &s, vector<int> &pos) {
        if (!filesystem::exists(this->filename)) {
            cout << "empty table!" << endl;
            return;
        } else {
            ifstream file(this->filename, ios::in | ios::binary);
            file.seekg(0, ios::end);
            int fileSize = file.tellg();
            file.seekg(0, ios::beg);
            char fData[fileSize];
            file.read(fData, sizeof(fData));
            file.close();
            for (auto i = 0uz; i < pos.size(); ++i) {
                if (pos[i] == -1) {
                    s[i] = "";
                    continue;
                }
                int offset = pos[i] * (maxRecSize + 1);
                //file.seekg(offset + 1, ios::beg);
                uint16_t recSize = 0;
                memcpy((char *)&recSize, fData + offset + 1, sizeof(recSize));
                //file.read((char *)&recSize, sizeof(recSize));
                char _res[recSize];
                //file.seekg(1, ios::cur);
                memcpy(_res, fData + offset + 2 + sizeof(recSize), sizeof(_res));
                //file.read(_res, sizeof(_res));
                s[i] = string(_res, sizeof(_res));
            }
        }
    }

    /**
    * @brief   在磁盘上更新表记录
    * @param   pos
    * @param   s
    */
    void updateRecord(int pos, string s) {
        vector<record> recs;
        makeRecords(this->database, this->table, recs, s);
        if (recs.size() > 1) {
            cout << "out of bounds!" << endl;
            return;
        }
        if (!filesystem::exists(this->filename)) {
            cout << "empty table!" << endl;
            return;
        } else {
            fstream file(this->filename, ios::in | ios::out | ios::binary);

            int offset = pos * (maxRecSize + 1);
            file.seekp(offset, ios::beg);
            recs[0].save(file);
            file.seekp(ios::beg);
            file.close();
        }
    }
    void updateRecord(vector<int> &poses, vector<string> &s) {
        if (!filesystem::exists(this->filename)) {
            cout << "empty table!" << endl;
            return;
        }
        string profFilename = dataPos + database + "/" + table + ".prof";
        ifstream profFile(profFilename, ios::in | ios::binary);
        profFile.seekg(0, ios::end);
        int profFileSize = profFile.tellg();
        profFile.seekg(0, ios::beg);
        char fData[profFileSize];
        profFile.read(fData, sizeof(fData));
        profFile.close();

        fstream file(this->filename, ios::in | ios::out | ios::binary);
        for (auto i = 0uz; i < poses.size(); ++i) {
            if (s[i] == "") {
                continue;
            }
            vector<record> recs;
            makeRecords(this->database, this->table, recs, s[i], fData);
            if (recs.size() > 1) {
                cout << "out of bounds!" << endl;
                return;
            }

            int offset = poses[i] * (maxRecSize + 1);
            file.seekp(offset, ios::beg);
            recs[0].save(file);
        }
        file.close();
    }

    /**
    * @brief   在磁盘上删除表记录
    * @param   pos
    * @return  true
    * @return  false
    */
    bool deleteRecord(int pos) {
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
    bool deleteRecord(vector<int> &poses, vector<bool> &erased) {
        fstream _fi(this->filename, ios::in | ios::out | ios::binary);
        for (auto i = 0uz; i < poses.size(); ++i) {
            if (poses[i] == -1 || erased[i] == false) {
                continue;
            }
            int offset = poses[i] * (maxRecSize + 1);
            _fi.seekp(offset, ios::beg);
            char blank[maxRecSize];
            memset(blank, 0, maxRecSize);
            _fi.write(blank, maxRecSize);
        }
        _fi.close();

        return true;
    }

    /**
    * @brief   无条件初始化磁盘数据
    */
    void profInit() {
        string prof = dataPos + database + "/" + table + ".prof";
        if (!filesystem::exists(filename)) {
            fstream file(filename, ios::binary | ios::out);
            file.seekp(maxPageSize - 1);
            file.write("", 1);

            fstream _file(prof, ios::binary | ios::out | ios::in);
            _file.seekp(maxPropSize, ios::beg);
            int fptr = 0;
            _file.write((char *)&fptr, 4);
            _file.seekp(ios::beg);
            _file.close();
            file.seekp(ios::beg);
            file.close();
        }
    }

    void renew() {
        string filename = dataPos + "db/" + "table.dat";
        string database = "db";
        string table = "table";
        keyType = 1;
    }
private:
    /**
    * @brief   在内存中实例化一个记录
    * @param   database
    * @param   tablename
    * @param   recs
    * @param   s
    */
    void makeRecords(string const database, string const tablename, vector<record> &recs, string s) {
        vector<string> _split;
        str_split(s, _split, regex("((\"\\s*,\\s*\")|(\"\\s*,\\s*)|(\\s*,\\s*\")|(\\s*,\\s*))"));
        string filename = dataPos + database + "/" + tablename + ".prof";
        ifstream file(filename, ios::in | ios::binary);
        file.seekg(8, ios::beg);
        char _r[4] = {};
        file.read(_r, 4);
        int propNums = 0;
        memcpy(&propNums, _r, 4);
        if (propNums != (int)_split.size()) {
            cout << "data mismatch!" << endl;
            file.close();
            return;
        }
        for (auto &i : _split) {
            char _signal = 0;
            file.read(&_signal, 1);
            char _type = (_signal == 0 ? 's' : 'i');
            i.insert(i.begin(), _type);
        }
        file.close();
        if (_split.front()[1] == '\"')
            _split.front().erase(1, 1);
        if (_split.back().back() == '\"')
            _split.back().pop_back();

        auto beg = 0uz;
        int cnts = 0;
        while (beg < _split.size()) {
            record rec;
            beg = rec.fill(_split);
            recs.push_back(rec);
            cnts++;
            if (beg < _split.size()) {
                cout << "data out of bounds!" << endl;
                remove((dataPos + database + "/" + tablename + ".dat").c_str());
                remove((dataPos + database + "/" + tablename + ".ind").c_str());
                remove((dataPos + database + "/" + tablename + ".prof").c_str());
                exit(1);
            }
        }
        recs.front().make_head(cnts);
    }
    void makeRecords(string const database, string const tablename, vector<record> &recs, string s, char* fData) {
        vector<string> _split;
        str_split(s, _split, regex("((\"\\s*,\\s*\")|(\"\\s*,\\s*)|(\\s*,\\s*\")|(\\s*,\\s*))"));
        int filePtr = 0;
        char _r[4] = {};
        filePtr += 8;
        memcpy(_r, fData + filePtr, 4);
        filePtr += 4;
        int propNums = 0;
        memcpy(&propNums, _r, 4);
        if (propNums != (int)_split.size()) {
            cout << "data mismatch!" << endl;
            return;
        }
        for (auto &i : _split) {
            char _signal = 0;
            memcpy(&_signal, fData + filePtr, 1);
            filePtr += 1;
            char _type = (_signal == 0 ? 's' : 'i');
            i.insert(i.begin(), _type);
        }
        if (_split.front()[1] == '\"')
            _split.front().erase(1, 1);
        if (_split.back().back() == '\"')
            _split.back().pop_back();

        auto beg = 0uz;
        int cnts = 0;
        while (beg < _split.size()) {
            record rec;
            beg = rec.fill(_split);
            recs.push_back(rec);
            cnts++;
            if (beg < _split.size()) {
                cout << "data out of bounds!" << endl;
                remove((dataPos + database + "/" + tablename + ".dat").c_str());
                remove((dataPos + database + "/" + tablename + ".ind").c_str());
                remove((dataPos + database + "/" + tablename + ".prof").c_str());
                exit(1);
            }
        }
        recs.front().make_head(cnts);
    }

    /**
    * @brief   将内存中的记录数据存储到磁盘
    * @param   database
    * @param   tablename
    * @param   recs
    */
    void makeTable(string const database, string const tablename, vector<record> &recs) {
        string filename = dataPos + database + "/" + tablename + ".dat";
        if (!filesystem::exists(filename)) {
            fstream file(filename, ios::binary | ios::out);
            int cnts = 0;
            int fptr = 0;
            for (auto i = 0uz; i < recs.size(); ++i) {
                if (recs.size() > 1 && i != recs.size() - 1) {
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
            _file.write((char *)&fptr, 4);
            _file.seekp(ios::beg);
            _file.close();
            file.seekp(ios::beg);
            file.close();
        } else {
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
            for (auto i = 0uz; i < recs.size(); ++i) {
                if (recs.size() > 1 && i != recs.size() - 1) {
                    recs[i].data[3] = cnts + 1;
                }
                recs[i].save(file);
                cnts++;
                fptr = file.tellp();
                file.seekp(maxPageSize * cnts - 1);
                file.write("", 1);
            }
            _file.seekp(maxPropSize, ios::beg);
            _file.write((char *)&fptr, 4);
            _file.seekp(ios::beg);
            _file.close();
            file.seekp(ios::beg);
            file.close();
        }
    }

};
}