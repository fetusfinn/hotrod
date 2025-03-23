//
//	print.h | Finn Le Var
//
#pragma once

#include <print>
#include <format>

#define _macro(_code) do { _code } while(0)

#define PRINT_PREFIX "hot"

#define printprefix(_msg, _prefix)	_macro( std::print("[{}] {}\n", _prefix, _msg);		)
#define printerror(_msg)			_macro( std::print(stderr, "[error] {}\n", _msg);	)
#define printmsg(_msg)				printprefix(_msg, PRINT_PREFIX)
#define printdebug(_msg)			printprefix(_msg, "debug")
#define printerrorfunc(_msg)		printerror(std::format("{} | {}", __FUNCTION__, _msg))

// prints and returns out of the current function
#define printerret(_msg, _ret) { printerrorfunc(_msg); return _ret; }