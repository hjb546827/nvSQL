/**
 * @file		DML.h
 * @brief	    INSERT、UPDATE、DELETE
 * @author		hjb
 * @version		1.0
 * @date		2023-11-27
 * @copyright	Copyright (c) 2023
 */

#pragma once

#include <iostream>
#include <initializer_list>
#include <format>
#include <vector>
#include <regex>
#include "base.h"
#include "table.h"

/**
 * @brief	数据库操纵语句
 */
namespace DML {
    /**
     * @brief	插入记录
     * @param	database    数据库名
     * @param	tablename   表名
     * @param	cmd         终端输入
     * @return	true        成功
     * @return	false       失败
     */
    bool insertRecord(const std::string& database, const std::string& tablename, const std::string& cmd);
    /**
     * @brief	更新记录
     * @param	database    数据库名
     * @param	tablename   表名
     * @param	cmd         终端输入
     * @return	true        成功
     * @return	false       失败
     */
    bool updateRecord(const std::string& database, const std::string& tablename, const std::string& cmd);
    /**
     * @brief	删除记录
     * @param	database    数据库名
     * @param	tablename   表名
     * @param	cmd         终端输入
     * @return	true        成功
     * @return	false       失败
     */
    bool deleteRecord(const std::string& database, const std::string& tablename, const std::string& cmd);
}