//
//	test.h | Finn Le Var
//
#pragma once

#include <string>

struct module_context_t;

//
//
//
namespace test
{
	void dump(const module_context_t* _mod);
	void print(const std::string& _str);
}
