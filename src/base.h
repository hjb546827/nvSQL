/**
 * @file		base.h
 * @brief
 * @author		hjb
 * @version		1.0
 * @date		2023-11-02
 * @copyright	Copyright (c) 2023
 */

#pragma once

#include <iostream>
#include <initializer_list>
#include <format>

static const char *br = "\n";

namespace ioc
{
    enum WRITE_MODE
    {
        _DEFAULT,
        _APP
    };
    using w_mode = WRITE_MODE;

    static const w_mode out = (w_mode)0; // 覆盖写入
    static const w_mode app = (w_mode)1; // 追加写入
}