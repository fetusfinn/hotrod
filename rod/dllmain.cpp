//
//	test dll for the hotloader
//
#define HOT_DEBUG			// enable debug build for this module
#define HOT_MOD		"mod"	// tell the project we're a module, also the name of this module

#include <thread>
#include <utility>

#include "shared/context.h"
#include "shared/macros.h"		// for HOT_EXPORT
#include "shared/print.h"
#include "shared/subsystem.h"

// for functions that we're exporting to the main program
#define HOT_EXPORT extern "C" _declspec(dllexport)

//
// static vars
//
namespace
{
	// our engine context
	engine_context_t* g_engine = nullptr;

	// this modules context
	module_context_t* g_mod = nullptr;
}

//
// called when our module is loaded into the engine to initialise our module
// returns true if successfully loaded and initialised
//
bool MOD_INIT_FN(engine_context_t* _ctx)
{
	if (!_ctx)
		printerret(false, "no engine context");

	// store the context
	g_engine = _ctx;

	// initialise stuff

	// initialise our subsys manager and parse the subsystems in our engine ctx
	// this could maybe be done in the engine
	g_subsystem.init(_ctx);

	sub_test_ctx_t* test = g_subsystem.find<sub_test_ctx_t>(SUB_TEST);

	if (test)
	{
		test->print("test, hello");

		SLEEPFOR(500);

		if (g_mod)
			test->dump(g_mod);
	}

	printmsg("initialised");

	return g_engine != nullptr;
}

//
// called on input
//
void MOD_INPUT_FN()
{
	DO_ONCE(printmsg("on_input"));

	// use input context from engine
}

//
// called on update
//
void MOD_UPDATE_FN()
{
	//DO_ONCE(printmsg("on_update"));

	//printmsg("on_update");

	printmsg("dllzNUTZ");
	printmsg("haaaah");
	printmsg("GOTTEEEM");
}

//
// called when our module is unloaded from the engine
// returns true if everything successfully shutdown
//
bool MOD_UNLOAD_FN()
{
	DO_ONCE(printmsg("on_unload"));

	// todo : unload everything

	g_mod	 = nullptr;
	g_engine = nullptr;

	// succesfully unloaded
	return true;
}

//
// called when this module needs to internally reload
//
void MOD_RELOAD_FN()
{
	DO_ONCE(printmsg("on_reload"));

	// reset internal vars and stuff
}

//
// our export function
//
HOT_EXPORT void MOD_LOAD_FN(module_context_t* _mod)
{
	// the engine must create and pass _mod to our module
	if (!_mod)
		return;

	// set our contexts vars
	_mod->name		= HOT_MOD;
	_mod->author	= "finn";
	_mod->desc		= "test module";
	_mod->major		= 0;
	_mod->minor		= 1;

	// store our module's func in the context so that the engine can access and use them
	_mod->CTX_INIT_FN	= &MOD_INIT_FN;
	_mod->CTX_INPUT_FN  = &MOD_INPUT_FN;
	_mod->CTX_UPDATE_FN = &MOD_UPDATE_FN;
	_mod->CTX_UNLOAD_FN = &MOD_UNLOAD_FN;
	// _mod->CTX_RELOAD_FN	= &MOD_RELOAD_FN;	// not implemented so dont need to set it

	// if on_init, on_input, on_update, and on_unload were set then the module is considered loaded
	_mod->loaded = _mod->CTX_INIT_FN && _mod->CTX_INPUT_FN && _mod->CTX_UPDATE_FN && _mod->CTX_UNLOAD_FN;

	// store our mod
	g_mod = _mod;

	printfunc("context setup");
}

//
// old code
// 
//#include <print>
//#include <thread>
//
//#define HOT_EXPORT extern "C" _declspec(dllexport)
//
////
//// the function we're gonna be loading from our program
////
//HOT_EXPORT void dllmain()
//{
//	std::print("uhhhh\n");
//	std::this_thread::sleep_for(std::chrono::milliseconds(500));
//	std::print("ummmmmm\n");
//	std::this_thread::sleep_for(std::chrono::milliseconds(500));
//	std::print("uuhhmmmm\n");
//
//	return;
//
//	std::print("dllz NUTZ\n");
//	std::this_thread::sleep_for(std::chrono::milliseconds(500));
//	std::print("HAH\n");
//	std::this_thread::sleep_for(std::chrono::milliseconds(500));
//	std::print("GOTTEEM\n");
//}