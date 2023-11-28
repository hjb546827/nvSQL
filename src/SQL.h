/**
 * @file		SQL.h
 * @brief	
 * @author		hjb
 * @version		1.0
 * @date		2023-11-27
 * @copyright	Copyright (c) 2023
 */

#pragma once

#include "base.h"
#include "table.h"

/**
 * @brief   数据定义语句
 */
namespace DDL {
    /**
    * @brief   创建数据库
    * @param   database    数据库名
    * @return  true        成功
    * @return  false       失败
    */
    bool createDatabase(const std::string &database);
    /**
    * @brief   删除数据库
    * @param   database    数据库名
    * @return  true        成功
    * @return  false       失败
    */
    bool dropDatabase(const std::string &database);
    /**
    * @brief   创建表
    * @param   database    数据库名
    * @param   tablename   表名
    * @param   cmd         终端输入
    * @return  true        成功
    * @return  false       失败
    */
    bool createTable(const std::string &database, const std::string &tablename, const string &cmd);
    /**
    * @brief   删除表
    * @param   database    数据库名
    * @param   tablename   表名
    * @return  true        成功
    * @return  false       失败
    */
    bool dropTable(const std::string &database, const std::string &tablename);
}

namespace DML {
    /**
    * @brief   插入记录
    * @param   database    数据库名
    * @param   tablename   表名
    * @param   cmd         终端输入
    * @return  true        成功
    * @return  false       失败
    */
    bool insertRecord(const std::string &database, const std::string &tablename, const std::string &cmd);
    /**
    * @brief   更新记录
    * @param   database    数据库名
    * @param   tablename   表名
    * @param   cmd         终端输入
    * @return  true        成功
    * @return  false       失败
    */
    bool updateRecord(const std::string &database, const std::string &tablename, const std::string &cmd);
    /**
    * @brief   删除记录
    * @param   database    数据库名
    * @param   tablename   表名
    * @param   cmd         终端输入
    * @return  true        成功
    * @return  false       失败
    */
    bool deleteRecord(const std::string &database, const std::string &tablename, const std::string &cmd);
}

/**
 * @brief	数据查询语句
 */
namespace DQL {
    bool selectRecord(const std::string& database, const std::vector<std::string>& res, const std::string& cmd);
}