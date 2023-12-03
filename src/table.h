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
#include <string>
#include <vector>

/**
 * @brief   数据表
 * @tparam  T 表的主键类型int/string
 */
template <typename T = int>
class table {
private:
    std::string dataFilename;                // .dat文件路径
    std::string profFilename;                // .prof文件路径
    tPropTypeList_t props;   // 表的属性列表
    bpT::bpTree<T> t;                        // 表索引结构

protected:
    /**
     * @brief   删除多个数据
     * @param   key         主键值
     * @param   conditions  where条件列表
     * @return  true        成功
     * @return  false       失败
     */
    bool erase_some(std::string &_record, tCdtPosList_t &conditions,
                    std::vector<bool> &erased, std::vector<char> &opers) {
        if (_record == "") {
            return false;
        }
        char _rec[_record.size()];
        memcpy(_rec, _record.data(), _record.size());
        int _nowPos = 0;
        int _nowSize = 0;
        std::vector<std::string> fullData;
        for (auto i = 0uz; i < props.size(); ++i) {
            if (props[i].second == 1) {
                int _iInt = 0;
                memcpy((char *)&_iInt, _rec + _nowPos + 4, 4);
                fullData.push_back(std::to_string(_iInt));
                _nowPos += 8;
            } else {
                memcpy((char *)&_nowSize, _rec + _nowPos, 4);
                char _iStr[_nowSize + 1];
                memcpy(_iStr, _rec + _nowPos + 4, _nowSize);
                _iStr[_nowSize] = '\0';
                fullData.push_back(std::string(_iStr));
                _nowPos += 4 + _nowSize;
            }
        }
        for (auto i = 0uz; i < conditions.size(); ++i) { // > : 0; < : 1; = : 2; >= : 3; <= : 4;
            switch (opers[i]) {
            case 0: // >
                if (fullData[conditions[i].first] <= conditions[i].second) {
                    return true;
                }
                break;
            case 1: // <
                if (fullData[conditions[i].first] >= conditions[i].second) {
                    return true;
                }
                break;
            case 2: // =
                if (fullData[conditions[i].first] != conditions[i].second) {
                    return true;
                }
                break;
            case 3: // >=
                if (fullData[conditions[i].first] < conditions[i].second) {
                    return true;
                }
                break;
            case 4: // <=
                if (fullData[conditions[i].first] > conditions[i].second) {
                    return true;
                }
                break;
            }
        }
        erased.back() = true;
        return true;
    }

    /**
     * @brief   读取多个数据
     * @param   key         主键值
     * @param   widths      属性最大数据长度
     * @param   properties  读取属性列表
     * @param   datas       输出数据
     * @param   conditions  where条件列表
     * @return  true        成功
     * @return  false       失败
     */
    inline bool read_some(std::string &_record, std::vector<int> &widths, std::vector<int> &properties,
                          printData_t &datas, tCdtPosList_t &conditions, std::vector<char> &opers) {
        if (_record == "") {
            return false;
        }
        char _rec[_record.size()];
        memcpy(_rec, _record.data(), _record.size());
        int _nowPos = 0;
        int _nowSize = 0;
        std::vector<std::string> fullData;
        for (auto i = 0uz; i < props.size(); ++i) {
            if (props[i].second == 1) {
                int _iInt = 0;
                memcpy((char *)&_iInt, _rec + _nowPos + 4, 4);
                fullData.push_back(std::to_string(_iInt));
                _nowPos += 8;
            } else {
                memcpy((char *)&_nowSize, _rec + _nowPos, 4);
                char _iStr[_nowSize + 1];
                memcpy(_iStr, _rec + _nowPos + 4, _nowSize);
                _iStr[_nowSize] = '\0';
                fullData.push_back(std::string(_iStr));
                _nowPos += 4 + _nowSize;
            }
        }
        for (auto i = 0uz; i < conditions.size(); ++i) { // > : 0; < : 1; = : 2; >= : 3; <= : 4;
            switch (opers[i]) {
            case 0: // >
                if (fullData[conditions[i].first] <= conditions[i].second) {
                    return true;
                }
                break;
            case 1: // <
                if (fullData[conditions[i].first] >= conditions[i].second) {
                    return true;
                }
                break;
            case 2: // =
                if (fullData[conditions[i].first] != conditions[i].second) {
                    return true;
                }
                break;
            case 3: // >=
                if (fullData[conditions[i].first] < conditions[i].second) {
                    return true;
                }
                break;
            case 4: // <=
                if (fullData[conditions[i].first] > conditions[i].second) {
                    return true;
                }
                break;
            }
        }
        datas.push_back(std::vector<std::string> {});
        auto &data = datas.back();
        for (auto i = 0uz; i < properties.size(); ++i) {
            data.push_back(fullData[properties[i]]);
            widths[i] = std::max(widths[i], (int)data.back().size());
        }
        return true;
    }

    /**
     * @brief   更新多个数据
     * @param   key     主键值
     * @param   setCdt      set属性列表
     * @param   conditions  where条件列表
     * @return  true        成功
     * @return  false       失败
     */
    bool update_some(std::string &_record, std::string &_content, tCdtPos_t &setCdt,
                     tCdtPosList_t &conditions, std::vector<char> &opers) {
        if (_record == "") {
            return false;
        }
        char _rec[_record.size()];
        memcpy(_rec, _record.data(), _record.size());
        int _nowPos = 0;
        int _nowSize = 0;
        std::vector<std::string> fullData;
        for (auto i = 0uz; i < props.size(); ++i) {
            if (props[i].second == 1) {
                int _iInt = 0;
                memcpy((char *)&_iInt, _rec + _nowPos + 4, 4);
                fullData.push_back(std::to_string(_iInt));
                _nowPos += 8;
            } else {
                memcpy((char *)&_nowSize, _rec + _nowPos, 4);
                char _iStr[_nowSize + 1];
                memcpy(_iStr, _rec + _nowPos + 4, _nowSize);
                _iStr[_nowSize] = '\0';
                fullData.push_back(std::string(_iStr));
                _nowPos += 4 + _nowSize;
            }
        }
        for (auto i = 0uz; i < conditions.size(); ++i) { // > : 0; < : 1; = : 2; >= : 3; <= : 4;
            switch (opers[i]) {
            case 0: // >
                if (fullData[conditions[i].first] <= conditions[i].second) {
                    return true;
                }
                break;
            case 1: // <
                if (fullData[conditions[i].first] >= conditions[i].second) {
                    return true;
                }
                break;
            case 2: // =
                if (fullData[conditions[i].first] != conditions[i].second) {
                    return true;
                }
                break;
            case 3: // >=
                if (fullData[conditions[i].first] < conditions[i].second) {
                    return true;
                }
                break;
            case 4: // <=
                if (fullData[conditions[i].first] > conditions[i].second) {
                    return true;
                }
                break;
            }
        }
        fullData[setCdt.first] = setCdt.second;
        std::string content = "";
        for (auto i : fullData) {
            content += (i + ",");
        }
        if (content.back() == ',') {
            content.pop_back();
        }
        _content = content;
        return true;
    }

public:
    std::string database = ""; // 数据库名
    std::string name = "";  // 表名
    int primaryKey = 0;     // 主键在属性列表中位置

    table() = default;
    table(std::string const database, std::string const tablename) noexcept {
        init(database, tablename);
    }
    ~table() noexcept {}

    void init(std::string const database, std::string const tablename) noexcept {
        this->database = database;
        this->name = tablename;
        this->dataFilename = bpT::dataPos + database + "/" + tablename + ".dat";
        this->profFilename = bpT::dataPos + database + "/" + tablename + ".prof";

        if (std::filesystem::exists(profFilename)) {
            std::ifstream fi(profFilename, std::ios::in | std::ios::binary);
            fi.seekg(4, std::ios::beg);
            fi.read((char *)&primaryKey, 4);
            int _propNums = 0;
            fi.read((char *)&_propNums, 4);
            char _propType[_propNums];
            fi.read(_propType, sizeof(_propType));
            std::vector<std::string> _propName = {""};
            for (auto i = 0; i < _propNums - 1; ++i) {
                getline(fi, _propName[i], ' ');
                _propName.push_back("");
            }
            getline(fi, _propName.back(), (char)0);
            for (auto i = 0; i < _propNums; ++i) {
                props.push_back(tPropType_t {_propName[i], _propType[i]});
            }
            fi.close();
        }
    }

    void renew() {
        this->primaryKey = 0;
        this->props.clear();
        this->t.clear();
    }

    /**
     * @brief   在磁盘上创建表
     * @param   _props  属性列表
     * @param   pk      主键在属性列表中位置
     * @return  true    成功
     * @return  false   失败
     */
    bool createTable(tPropTypeList_t &_props, int pk = 0) {
        props.clear();
        primaryKey = pk;
        for (auto i : _props) {
            props.push_back(i);
        }
        std::fstream fi(profFilename, std::ios::out | std::ios::binary);
        fi.seekp(std::ios::beg);
        int keyIsInt = keyTypeIsString<T> == true ? 1 : 0;
        fi.write((char *)&keyIsInt, 4);
        fi.write((char *)&primaryKey, 4);
        int _propNums = props.size();
        if (_propNums == 0) {
            std::cout << "table volumn empty!" << std::endl;
            fi.close();
            return false;
        }
        fi.write((char *)&_propNums, 4);
        char _propType[_propNums];
        std::string _propName = "";
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

        t.init(database, name, bpT::bpTreeLevel);

        t.recordInit();

        return true;
    }

    /**
     * @brief   使用磁盘数据初始化内存中的表对象
     */
    void openTable() {
        t.init(database, name, bpT::bpTreeLevel);
    }

    /**
     * @brief   使用内存数据初始化内存中的表对象
     * @param   _props  属性列表
     */
    void openTable(tPropTypeList_t &_props) {
        props.clear();
        for (auto i : _props) {
            props.push_back(i);
        }

        t.init(database, name, bpT::bpTreeLevel);
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
     * @brief   读取数据
     * @param   widths      属性最大数据宽度
     * @param   properties  读取属性列表
     * @param   datas       输出数据
     * @param   conditions  where条件列表
     * @return  true        成功
     * @return  false       失败
     */
    bool readTable(std::vector<int> &widths, std::vector<std::string> &properties, printData_t &datas,
                   tCdtNameList_t &conditions) {
        if (t.head == nullptr)
            return false;
        if (t.head->leaf == nullptr)
            return false;
        if (t.head->leaf->vals.size() == 0)
            return false;

        std::vector<int> _props;
        tCdtPosList_t _cdts;
        std::vector<char> opers(conditions.size());
        int _pkCdt = -1;
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
        for (auto i = 0uz; i < conditions.size(); ++i) {
            for (auto j = 0uz; j < props.size(); ++j) {
                if (conditions[i].first == props[j].first) {
                    opers[i] = conditions[i].second.back();
                    conditions[i].second.pop_back();
                    _cdts.emplace_back(j, conditions[i].second);
                    if ((int)j == primaryKey) {
                        _pkCdt = i;
                    }
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
        // 索引查找
        if (_pkCdt > -1) {
            typename decltype(t)::key_type _pkKey;
            _pkKey = keyFormatConverter<typename decltype(t)::key_type>(conditions[_pkCdt].second)();
            std::vector<std::string> reses;
            t.find_matched(_pkKey, reses, opers[_pkCdt]);
            for (auto &res : reses) {
                read_some(res, widths, _props, datas, _cdts, opers);
            }
            return true;
        }
        // 全文查找
        std::vector<typename decltype(t)::key_type> keys;
        for (auto itr = t.head; itr != t.tail; itr = itr->next) {
            for (auto i : itr->leaf->vals) {
                keys.push_back(i.first);
            }
        }
        std::vector<std::string> reses(keys.size());
        t.find_some(keys, reses);
        for (auto &res : reses) {
            read_some(res, widths, _props, datas, _cdts, opers);
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
    bool updateTable(tCdtName_t &setCdt, tCdtNameList_t &conditions) {
        if (t.head == nullptr)
            return false;
        if (t.head->leaf == nullptr)
            return false;
        if (t.head->leaf->vals.size() == 0)
            return false;

        tCdtPosList_t _cdts;
        tCdtPos_t _setCdt;
        std::vector<char> opers(conditions.size());
        int _pkCdt = -1;
        for (auto i = 0uz; i < conditions.size(); ++i) {
            for (auto j = 0uz; j < props.size(); ++j) {
                if (conditions[i].first == props[j].first) {
                    opers[i] = conditions[i].second.back();
                    conditions[i].second.pop_back();
                    _cdts.emplace_back(j, conditions[i].second);
                    if ((int)j == primaryKey) {
                        _pkCdt = i;
                    }
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
        // 索引查找
        if (_pkCdt > -1) {
            typename decltype(t)::key_type _pkKey;
            _pkKey = keyFormatConverter<typename decltype(t)::key_type>(conditions[_pkCdt].second)();
            std::vector<std::string> reses;
            std::vector<int> poses;
            t.find_matched(_pkKey, reses, poses, opers[_pkCdt]);
            std::vector<std::string> contents(reses.size(), "");
            for (auto i = 0uz; i < reses.size(); ++i) {
                update_some(reses[i], contents[i], _setCdt, _cdts, opers);
            }
            t.update_some(contents, poses);
            return true;
        }
        // 全文查找
        std::vector<typename decltype(t)::key_type> keys;
        for (auto itr = t.head; itr != t.tail; itr = itr->next) {
            for (auto i : itr->leaf->vals) {
                keys.push_back(i.first);
            }
        }
        int sz = keys.size();
        std::vector<std::string> reses(sz);
        std::vector<std::string> contents(sz, "");
        std::vector<int> poses(sz, -1);
        t.find_some(keys, reses, poses);
        for (auto i = 0uz; i < reses.size(); ++i) {
            update_some(reses[i], contents[i], _setCdt, _cdts, opers);
        }
        t.update_some(contents, poses);
        return true;
    }

    /**
     * @brief   删除数据
     * @param   conditions  where条件列表
     * @return  true    成功
     * @return  false   失败
     */
    bool eraseTable(tCdtNameList_t &conditions) {
        if (t.head == nullptr)
            return false;
        if (t.head->leaf == nullptr)
            return false;
        if (t.head->leaf->vals.size() == 0)
            return false;

        tCdtPosList_t _cdts;
        std::vector<char> opers(conditions.size());
        int _pkCdt = -1;
        for (auto i = 0uz; i < conditions.size(); ++i) {
            for (auto j = 0uz; j < props.size(); ++j) {
                if (conditions[i].first == props[j].first) {
                    opers[i] = conditions[i].second.back();
                    conditions[i].second.pop_back();
                    _cdts.emplace_back(j, conditions[i].second);
                    if ((int)j == primaryKey) {
                        _pkCdt = i;
                    }
                    break;
                }
                if (j == props.size() - 1) {
                    return false;
                }
            }
        }
        // 索引查找
        if (_pkCdt > -1) {
            typename decltype(t)::key_type _pkKey;
            _pkKey = keyFormatConverter<typename decltype(t)::key_type>(conditions[_pkCdt].second)();
            std::vector<std::string> reses;
            std::vector<int> poses;
            std::vector<typename decltype(t)::key_type> keys;
            t.find_matched(_pkKey, keys, reses, poses, opers[_pkCdt]);
            int sz = poses.size();
            std::vector<bool> eraseds;
            for (auto i = 0; i < sz; ++i) {
                eraseds.push_back(false);
                erase_some(reses[i], _cdts, eraseds, opers);
            }
            t.erase(keys, poses, eraseds);
            return true;
        }
        // 全文查找
        std::vector<typename decltype(t)::key_type> keys;
        for (auto itr = t.head; itr != t.tail; itr = itr->next) {
            for (auto i : itr->leaf->vals) {
                keys.push_back(i.first);
            }
        }
        int sz = keys.size();
        std::vector<std::string> reses(sz);
        std::vector<std::string> contents(sz, "");
        std::vector<int> poses(sz, -1);
        std::vector<bool> eraseds;
        t.find_some(keys, reses, poses);
        for (auto i = 0; i < sz; ++i) {
            eraseds.push_back(false);
            erase_some(reses[i], _cdts, eraseds, opers);
        }
        t.erase(keys, poses, eraseds);
        return true;
    }

    /**
     * @brief   删除表
     * @param   database    数据库名
     * @param   tablename   表名
     * @return  true        成功
     * @return  false       失败
     */
    static bool dropTable(const std::string database, const std::string tablename) {
        std::string tablePos = bpT::dataPos + database + "/" + tablename;
        std::string dataFilename = tablePos + ".dat";
        std::string profFilename = tablePos + ".prof";
        std::string indexFilename = tablePos + ".ind";
        if (remove(profFilename.c_str()) != 0) {
            std::cout << "Table not exists!" << std::endl;
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
    static int getKeyType(const std::string database, const std::string tablename) {
        std::string filename = bpT::dataPos + database + "/" + tablename + ".prof";
        if (!std::filesystem::exists(filename)) {
            return -1;
        }
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        int keyType = 0;
        file.seekg(std::ios::beg);
        file.read((char *)&keyType, 4);
        file.close();

        return keyType;
    }
};