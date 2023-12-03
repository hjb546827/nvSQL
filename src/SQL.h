/**
 * @file        SQL.h
 * @brief
 * @author      hjb
 * @version     1.0
 * @date        2023-11-27
 * @copyright   Copyright (c) 2023
 */

#pragma once

#include "utility.h"
#include "table.h"
#include <vector>

/**
 * @brief   数据定义语句
 */
namespace DDL {
/**
* @brief   创建数据库
* @param   database    数据库名
* @param   times       计时器
* @return  true        成功
* @return  false       失败
*/
bool createDatabase(const std::string &database, CPUTimer &times);
/**
* @brief   删除数据库
* @param   database    数据库名
* @param   times       计时器
* @return  true        成功
* @return  false       失败
*/
bool dropDatabase(const std::string &database, CPUTimer &times);
/**
* @brief   创建表
* @param   database    数据库名
* @param   tablename   表名
* @param   cmd         终端输入
* @param   times       计时器
* @return  true        成功
* @return  false       失败
*/
bool createTable(const std::string &database, const std::string &tablename, const std::string &cmd,
                 cache<table> &indexCache, CPUTimer &times);
/**
* @brief   删除表
* @param   database    数据库名
* @param   tablename   表名
* @param   times       计时器
* @return  true        成功
* @return  false       失败
*/
bool dropTable(const std::string &database, const std::string &tablename, CPUTimer &times);
}

namespace DML {
/**
* @brief   插入记录
* @param   database    数据库名
* @param   tablename   表名
* @param   cmd         终端输入
* @param   times       计时器
* @return  true        成功
* @return  false       失败
*/
bool insertRecord(const std::string &database, const std::string &tablename, const std::string &cmd,
                  cache<table> &indexCache, CPUTimer &times);
/**
* @brief   更新记录
* @param   database    数据库名
* @param   tablename   表名
* @param   cmd         终端输入
* @param   times       计时器
* @return  true        成功
* @return  false       失败
*/
bool updateRecord(const std::string &database, const std::string &tablename, const std::string &cmd,
                  cache<table> &indexCache, CPUTimer &times);
/**
* @brief   删除记录
* @param   database    数据库名
* @param   tablename   表名
* @param   cmd         终端输入
* @param   times       计时器
* @return  true        成功
* @return  false       失败
*/
bool deleteRecord(const std::string &database, const std::string &tablename, const std::string &cmd,
                  cache<table> &indexCache, CPUTimer &times);
}

/**
 * @brief   数据查询语句
 */
namespace DQL {
/**
 * @brief   查询记录
 * @param   database    数据库名
 * @param   res         查询属性
 * @param   cmd         终端输入
 * @param   times       计时器
 * @return  true
 * @return  false
 */
bool selectRecord(const std::string &database, const std::vector<std::string> &res, const std::string &cmd,
                  cache<table> &indexCache, CPUTimer &times);
}