//
//	test.cpp | Finn Le Var
//
#include "test.h"

#include "shared/context.h"	// includes shared/macros.h

//
//
//
namespace test
{
	void dump(const module_context_t* _mod)
	{
		if (_mod)
		{
			printdebug("dumping module '" << _mod->name << "'");

			_mod->print_info();
		}
	}

	void print(const std::string& _str)
	{
		printmsg(_str);
	}
}