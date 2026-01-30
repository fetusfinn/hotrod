//
//	print.h | Finn Le Var
//
#pragma once

#include <iostream>
#include <format>

#define PRINT_PREFIX "hot"


#ifndef HOT_MOD // engine prints

#define printprefix(_msg, _prefix)	_macro( std::cout << "[" << _prefix << "] " << _msg << "\n"; )
#define printerror(_msg)			_macro( std::cerr << "[error] " << _msg << "\n"; )
#define printmsg(_msg)				printprefix(_msg, PRINT_PREFIX)
#define printdebug(_msg)			printprefix(_msg, "debug")
#define printfunc(_msg)				printmsg(std::format("{} | ", __FUNCTION__) << _msg)
#define printerrorfunc(_msg)		printerror(std::format("{} | ", __FUNCTION__) << _msg)

// prints and returns out of the current function
#define printerret(_ret, _msg)		_macro( printerrorfunc(_msg); return _ret; )


#else // module prints

//
// prints for our modules, will print something like "[mod][debug] our message"
//

#define printprefix(_msg, _prefix)	_macro( std::cout << "[" << HOT_MOD << "][" << _prefix << "] " << _msg << "\n"; )
#define printerror(_msg)			_macro( std::cerr << "[" << HOT_MOD << "][error] " << _msg << "\n"; )
#define printmsg(_msg)				printprefix(_msg, PRINT_PREFIX)
#define printdebug(_msg)			printprefix(_msg, "debug")
#define printfunc(_msg)				printmsg(std::format("{} | ", __FUNCTION__) << _msg)
#define printerrorfunc(_msg)		printerror(std::format("{} | ", __FUNCTION__) << _msg)

// prints and returns out of the current function
#define printerret(_ret, _msg)		_macro( printerrorfunc(_msg); return _ret; )

#endif
