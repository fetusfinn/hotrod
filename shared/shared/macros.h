//
//	macros.h | Finn Le Var
//
#pragma once

// debug macro
#define HOT_DEBUG

// wrapper macro so that we can use multi line code underneath statements
#define _macro(_code) do { _code; } while(0)

//
// helper macros
//

// sleeps the current thread for the given duration in milliseconds
#define SLEEPFOR(_ms) _macro(std::this_thread::sleep_for(std::chrono::milliseconds(_ms)))

// prints "Press any key to continue..." to the console and blocks until a key is pressed
#define WAITFORINPUT() _macro(std::system("PAUSE"))

// shorthand static and reinterpret cast
#define CASTTO(_type, _var) static_cast<_type>(_var)
#define RECAST(_type, _var) reinterpret_cast<_type>(_var)

// shorthand lock guard
#define LGUARD(_mutex)	std::lock_guard<std::mutex> _lock(_mutex)

// converts the given token to a string literal, stringizingizingizingizingizing or something lol
// making it two step so that we can use the values/results of macros such as MODULE_LOAD_FN,
// otherwise it stringises the macro rather than its value/result, which we don't want
#define TO_STRING(_t)		TO_STRING_IMPL(_t)
#define TO_STRING_IMPL(_t)	#_t

// combines the two given strings, two step for the same reason as above
#define CONCAT(_a, _b)		CONCAT_IMPL(_a, _b)
#define CONCAT_IMPL(_a, _b) _a##_b

//
// does the given action only once, mostly used for prints
//
#define DO_ONCE(_action)\
_macro(\
	static bool _print = false; \
	\
	if (!_print)\
	{\
		_action; \
		_print = true; \
	}\
)


//
// turns a class into a singleton by creating a static get instance func
// and by removing copy and move constructors
//
#define MAKE_SINGLETON(_class) \
static _class& getinst()\
{\
	static _class inst;\
	return inst;\
}\
_class(_class&&) = delete;\
_class(const _class&) = delete;\
_class& operator=(_class&&) = delete;\
_class& operator=(const _class&) = delete;

//
// creates a variable that stores a reference to the given singleton class's instance
// so that we dont have to type out the whole class_name_t::getinst() every time we want
// to access it
//
#define MAKE_SINGLETON_ALIAS(_class, _name) inline _class& CONCAT(g_, _name) = _class::getinst();

//
// module macros
//

// used in our module and engine
// putting these here as macros so that its easy to change them if we want
// todo : should probably not use macros for the release version

// the name of the load module func in our modules, the func that sets our module context for that module
// todo : maybe rename to 'get'
#define MOD_LOAD_FN		module_load
#define MOD_LOAD_STR	TO_STRING(MOD_LOAD_FN)

// the names of the functions in our modules that we want to pass to the engine
#define MOD_INIT_FN		on_load
#define MOD_INPUT_FN	on_input
#define MOD_UPDATE_FN	on_update
#define MOD_UNLOAD_FN	on_unload
#define MOD_RELOAD_FN	on_reload		// optional

// the suffix of our functions for our context definition
#define FN_SUFFIX _fn

// the names of the variables in our module context for the above functions
#define CTX_INIT_FN		CONCAT(MOD_INIT_FN,		FN_SUFFIX)
#define CTX_INPUT_FN	CONCAT(MOD_INPUT_FN,	FN_SUFFIX)
#define CTX_UPDATE_FN	CONCAT(MOD_UPDATE_FN,	FN_SUFFIX)
#define CTX_UNLOAD_FN	CONCAT(MOD_UNLOAD_FN,	FN_SUFFIX)
#define CTX_RELOAD_FN	CONCAT(MOD_RELOAD_FN,	FN_SUFFIX)

//
// print macros, including last so that it has access to all the above macros
//

#include "print.h"
