/**
 * @file		DQL.h
 * @brief	    SELECT
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
 * @brief	数据查询语句
 */
namespace DQL {
    bool selectRecord(const std::string& database, const std::vector<std::string>& res, const std::string& cmd);
}