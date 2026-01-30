//
//	util.h | Finn Le Var
//
#pragma once

#include <string>
#include <Windows.h>

//
//
//
namespace util
{
    std::string format_win32_error(DWORD _error_code);
}
