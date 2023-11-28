/**
 * @file        table.h
 * @brief       数据表
 * @author      hjb
 * @version     1.0
 * @date        2023-11-23
 * @copyright   Copyright (c) 2023
 */

#pragma once

#include "bpTree/bpTree.h"
#include <filesystem>
#include <fstream>
#include <vector>

/**
 * @brief   数据表
 * @tparam  T 表的主键类型int/string
 */
template <typename T = int>
class table {
private:
    string dataFilename;                // .dat文件路径
    string profFilename;                // .prof文件路径
    vector<pair<string, char>> props;   // 表的属性列表
    bpTree<T> t;                        // 表索引结构

protected:
    /**
     * @brief   删除单个数据
     * @param   key     主键值
     * @return  true    成功
     * @return  false   失败
     */
    bool erase(decltype(t)::key_type key) {
        bool ret = t.erase(key);
        t.save();
        return ret;
    }
    /**
     * @brief   删除单个数据
     * @param   key         主键值
     * @param   conditions  where条件列表
     * @return  true        成功
     * @return  false       失败
     */
    bool erase(decltype(t)::key_type key, vector<pair<int, string>> &conditions) {
        string _record = t.find(key);
        if (_record == "") {
            return false;
        }
        char _rec[_record.size()];
        memcpy(_rec, _record.data(), _record.size());
        int _nowPos = 0;
        int _nowSize = 0;
        vector<string> fullData;
        for (auto i = 0uz; i < props.size(); ++i) {
            if (props[i].second == 1) {
                int _iInt = 0;
                memcpy((char *)&_iInt, _rec + _nowPos + 4, 4);
                fullData.push_back(to_string(_iInt));
                _nowPos += 8;
            } else {
                memcpy((char *)&_nowSize, _rec + _nowPos, 4);
                char _iStr[_nowSize + 1];
                memcpy(_iStr, _rec + _nowPos + 4, _nowSize);
                _iStr[_nowSize] = '\0';
                fullData.push_back(string(_iStr));
                _nowPos += 4 + _nowSize;
            }
        }
        for (auto i : conditions) {
            if (fullData[i.first] != i.second) {
                return true;
            }
        }
        erase(key);
        return true;
    }

    /**
     * @brief   读取单个数据
     * @param   key         主键值
     * @param   widths      属性最大数据长度
     * @param   properties  读取属性列表
     * @param   datas       输出数据
     * @param   conditions  where条件列表
     * @return  true        成功
     * @return  false       失败
     */
    bool read(decltype(t)::key_type key, vector<int> &widths, vector<int> &properties,
              vector<vector<string>> &datas, vector<pair<int, string>> &conditions) {
        string _record = t.find(key);
        if (_record == "") {
            return false;
        }
        char _rec[_record.size()];
        memcpy(_rec, _record.data(), _record.size());
        int _nowPos = 0;
        int _nowSize = 0;
        vector<string> fullData;
        for (auto i = 0uz; i < props.size(); ++i) {
            if (props[i].second == 1) {
                int _iInt = 0;
                memcpy((char *)&_iInt, _rec + _nowPos + 4, 4);
                fullData.push_back(to_string(_iInt));
                _nowPos += 8;
            } else {
                memcpy((char *)&_nowSize, _rec + _nowPos, 4);
                char _iStr[_nowSize + 1];
                memcpy(_iStr, _rec + _nowPos + 4, _nowSize);
                _iStr[_nowSize] = '\0';
                fullData.push_back(string(_iStr));
                _nowPos += 4 + _nowSize;
            }
        }
        for (auto i : conditions) {
            if (fullData[i.first] != i.second) {
                return true;
            }
        }
        datas.push_back(vector<string> {});
        auto &data = datas.back();
        for (auto i = 0uz; i < properties.size(); ++i) {
            data.push_back(fullData[properties[i]]);
            widths[i] = max(widths[i], (int)data.back().size());
        }
        return true;
    }

    /**
     * @brief   更新单个数据
     * @param   key     主键值
     * @param   data    用于更新的数据
     * @return  true    成功
     * @return  false   失败
     */
    bool update(decltype(t)::key_type key, decltype(t)::keyValue::data_type data) {
        return t.update(key, data);
    }

    /**
     * @brief   更新单个数据
     * @param   key     主键值
     * @param   setCdt      set属性列表
     * @param   conditions  where条件列表
     * @return  true        成功
     * @return  false       失败
     */
    bool update(decltype(t)::key_type key, pair<int, string> &setCdt, vector<pair<int, string>> &conditions) {
        string _record = t.find(key);
        if (_record == "") {
            return false;
        }
        char _rec[_record.size()];
        memcpy(_rec, _record.data(), _record.size());
        int _nowPos = 0;
        int _nowSize = 0;
        vector<string> fullData;
        for (auto i = 0uz; i < props.size(); ++i) {
            if (props[i].second == 1) {
                int _iInt = 0;
                memcpy((char *)&_iInt, _rec + _nowPos + 4, 4);
                fullData.push_back(to_string(_iInt));
                _nowPos += 8;
            } else {
                memcpy((char *)&_nowSize, _rec + _nowPos, 4);
                char _iStr[_nowSize + 1];
                memcpy(_iStr, _rec + _nowPos + 4, _nowSize);
                _iStr[_nowSize] = '\0';
                fullData.push_back(string(_iStr));
                _nowPos += 4 + _nowSize;
            }
        }
        for (auto i : conditions) {
            if (fullData[i.first] != i.second) {
                return true;
            }
        }
        fullData[setCdt.first] = setCdt.second;
        string content = "";
        for (auto i : fullData) {
            content += (i + ",");
        }
        if (content.back() == ',') {
            content.pop_back();
        }
        update(key, content);
        return true;
    }

public:
    string database = "db"; // 数据库名
    string name = "table";  // 表名
    int primaryKey = 0;     // 主键在属性列表中位置

    table() = delete;
    table(string const database, string const tablename) noexcept {
        this->database = database;
        this->name = tablename;
        this->dataFilename = dataPos + database + "/" + tablename + ".dat";
        this->profFilename = dataPos + database + "/" + tablename + ".prof";

        if (filesystem::exists(profFilename)) {
            ifstream fi(profFilename, ios::in | ios::binary);
            fi.seekg(4, ios::beg);
            fi.read((char *)&primaryKey, 4);
            int _propNums = 0;
            fi.read((char *)&_propNums, 4);
            char _propType[_propNums];
            fi.read(_propType, sizeof(_propType));
            vector<string> _propName = {""};
            for (auto i = 0; i < _propNums - 1; ++i) {
                getline(fi, _propName[i], ' ');
                _propName.push_back("");
            }
            getline(fi, _propName.back(), (char)0);
            for (auto i = 0; i < _propNums; ++i) {
                props.push_back(pair<string, int> {_propName[i], _propType[i]});
            }
            fi.close();
        }
    }
    ~table() noexcept {}

    /**
     * @brief   在磁盘上创建表
     * @param   _props  属性列表
     * @param   pk      主键在属性列表中位置
     * @return  true    成功
     * @return  false   失败
     */
    bool createTable(vector<pair<string, char>> &_props, int pk = 0) {
        props.clear();
        primaryKey = pk;
        for (auto i : _props) {
            props.push_back(i);
        }
        fstream fi(profFilename, ios::out | ios::binary);
        fi.seekp(ios::beg);
        int keyIsInt = keyTypeIsString<T> == true ? 1 : 0;
        fi.write((char *)&keyIsInt, 4);
        fi.write((char *)&primaryKey, 4);
        int _propNums = props.size();
        if (_propNums == 0) {
            cout << "table volumn empty!" << endl;
            fi.close();
            return false;
        }
        fi.write((char *)&_propNums, 4);
        char _propType[_propNums];
        string _propName = "";
        for (auto i = 0; i < _propNums; ++i) {
            _propType[i] = props[i].second;
            _propName += props[i].first;
            _propName += " ";
        }
        _propName.pop_back();
        char _writeIn[_propName.size() + 1];
        memset(_writeIn, 0, sizeof(_writeIn));
        memcpy(_writeIn, _propName.data(), _propName.size());
        fi.write(_propType, sizeof(_propType));
        fi.write(_writeIn, sizeof(_writeIn));

        fi.close();

        t.init(database, name, bpTreeLevel);

        t.recordInit();

        return true;
    }

    /**
     * @brief   使用磁盘数据初始化内存中的表对象
     */
    void openTable() {
        t.init(database, name, bpTreeLevel);
    }

    /**
     * @brief   使用内存数据初始化内存中的表对象
     * @param   _props  属性列表
     */
    void openTable(vector<pair<string, char>> &_props) {
        props.clear();
        for (auto i : _props) {
            props.push_back(i);
        }

        t.init(database, name, bpTreeLevel);
    }

    /**
     * @brief   获得主键在属性列表中位置
     * @return  int 主键在属性列表中位置
     */
    int getPrimaryKey() {
        return primaryKey;
    }

    /**
     * @brief   插入数据
     * @param   v   主键值
     */
    void insertTable(decltype(t)::keyValue v) {
        t.insert(v);
        t.save();
    }

    /**
     * @brief   删除数据
     * @param   conditions  where条件列表
     * @return  true    成功
     * @return  false   失败
     */
    bool eraseTable(vector<pair<string, string>> &conditions) {
        if (t.head == nullptr)
            return false;
        if (t.head->leaf == nullptr)
            return false;
        if (t.head->leaf->vals.size() == 0)
            return false;

        vector<pair<int, string>> _cdts;
        for (auto i : conditions) {
            for (auto j = 0uz; j < props.size(); ++j) {
                if (i.first == props[j].first) {
                    _cdts.emplace_back(j, i.second);
                    break;
                }
                if (j == props.size() - 1) {
                    return false;
                }
            }
        }

        for (auto itr = t.head; itr != t.tail; itr = itr->next) {
            for (auto i : itr->leaf->vals) {
                erase(i.first, _cdts);
            }
        }
        return true;
    }

    /**
     * @brief   读取数据
     * @param   widths      属性最大数据宽度
     * @param   properties  读取属性列表
     * @param   datas       输出数据
     * @param   conditions  where条件列表
     * @return  true        成功
     * @return  false       失败
     */
    bool readTable(vector<int> &widths, vector<string> &properties, vector<vector<string>> &datas,
                   vector<pair<string, string>> &conditions) {
        if (t.head == nullptr)
            return false;
        if (t.head->leaf == nullptr)
            return false;
        if (t.head->leaf->vals.size() == 0)
            return false;

        vector<int> _props;
        vector<pair<int, string>> _cdts;
        for (auto i : properties) {
            for (auto j = 0uz; j < props.size(); ++j) {
                if (i == props[j].first) {
                    _props.push_back(j);
                    break;
                }
                if (j == props.size() - 1) {
                    return false;
                }
            }
        }
        for (auto i : conditions) {
            for (auto j = 0uz; j < props.size(); ++j) {
                if (i.first == props[j].first) {
                    _cdts.emplace_back(j, i.second);
                    break;
                }
                if (j == props.size() - 1) {
                    return false;
                }
            }
        }
        if (properties.size() == 0) {
            for (auto i = 0uz; i < props.size(); ++i) {
                _props.push_back(i);
                properties.push_back(props[i].first);
            }
        }

        for (auto i : _props)
            widths.push_back(props[i].first.size());
        for (auto itr = t.head; itr != t.tail; itr = itr->next) {
            for (auto i : itr->leaf->vals) {
                read(i.first, widths, _props, datas, _cdts);
            }
        }
        return true;
    }

    /**
     * @brief   更新数据
     * @param   setCdt      set属性列表
     * @param   conditions  where条件列表
     * @return  true        成功
     * @return  false       失败
     */
    bool updateTable(pair<string, string> &setCdt, vector<pair<string, string>> &conditions) {
        if (t.head == nullptr)
            return false;
        if (t.head->leaf == nullptr)
            return false;
        if (t.head->leaf->vals.size() == 0)
            return false;

        vector<pair<int, string>> _cdts;
        pair<int, string> _setCdt;
        for (auto i : conditions) {
            for (auto j = 0uz; j < props.size(); ++j) {
                if (i.first == props[j].first) {
                    _cdts.emplace_back(j, i.second);
                    break;
                }
                if (j == props.size() - 1) {
                    return false;
                }
            }
        }
        for (auto i = 0uz; i < props.size(); ++i) {
            if (setCdt.first == props[i].first) {
                _setCdt.first = i;
                _setCdt.second = setCdt.second;
                break;
            }
            if (i == props.size() - 1) {
                return false;
            }
        }

        for (auto itr = t.head; itr != t.tail; itr = itr->next) {
            for (auto i : itr->leaf->vals) {
                update(i.first, _setCdt, _cdts);
            }
        }
        return true;
    }

    /**
     * @brief   删除表
     * @param   database    数据库名
     * @param   tablename   表名
     * @return  true        成功
     * @return  false       失败
     */
    static bool dropTable(const string database, const string tablename) {
        string tablePos = dataPos + database + "/" + tablename;
        string dataFilename = tablePos + ".dat";
        string profFilename = tablePos + ".prof";
        string indexFilename = tablePos + ".ind";
        if (remove(profFilename.c_str()) != 0) {
            cout << "Table not exists!" << endl;
            return false;
        }
        remove(dataFilename.c_str());
        remove(indexFilename.c_str());

        return true;
    }

    /**
     * @brief   从磁盘中获取主键在属性列表中的位置
     * @param   database    数据库名
     * @param   tablename   表名
     * @return  int         主键在属性列表中的位置
     */
    static int getKeyType(string const database, string const tablename) {
        string filename = dataPos + database + "/" + tablename + ".prof";
        if (!filesystem::exists(filename)) {
            return -1;
        }
        ifstream file(filename, ios::in | ios::binary);
        int keyType = 0;
        file.seekg(ios::beg);
        file.read((char *)&keyType, 4);
        file.close();

        return keyType;
    }
};