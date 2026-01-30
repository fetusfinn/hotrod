//
//	context.h | Finn Le Var
//
#pragma once

#include <cstdint>
#include <functional>

#include "macros.h"

//
// required defs
//

// forward decl
struct module_context_t;

// render function sig alias, had to move here, kept getting errors
using imrenderfn_t = std::function<void()>;

//
// subsystem contexts
//

//
// test subsystem context
//
struct sub_test_ctx_t
{
	void (*dump)(module_context_t*);
	void (*print)(const std::string&);
};

//
// imgui subsystem context definition (must match engine's definition)
//
struct sub_imgui_ctx_t
{
	void (*register_fn)(const imrenderfn_t&);
	void (*clear_fns)();

	// the main app's imgui context - modules MUST call ImGui::SetCurrentContext(ctx)
	// before using any ImGui functions, as DLLs have their own static GImGui pointer
	void* ctx;
};

//
// wrapper for our contexts so that we can store them in a single array rather than
// all individually, also makes it so that our engine context only contains valid
// subsystems so that we don't get confused with or have to check what's valid or not
//
struct subsystem_info_t
{
	const char* name;
	void*		data;			// cast to input_context_t*, etc.
	// uint32_t	version;		// dont really need atm
};

//
// engine context
//

//
// our engine context that stores all of our subsystems
//
struct engine_context_t
{
	// array of all of our subsystems
	subsystem_info_t* subsystems;

	// the size of our subsystem array, aka how many we have
	uint8_t subsystem_count;
};

//
// we dont want this in our modules, it will be passed to them
//
#ifndef HOT_MOD

	// our engine context, to be sent to our modules, putting here to make it global for now
	// todo : move elsewhere, just here for now
	extern engine_context_t g_engine;

#endif

//
// module context
//
// todo : need to create custom contexts for each module that we create, then use a void* to pass that context via on_load()
// todo : maybe have a base context that has all the generic stuff that is shared, then all other contexts inherit from that and add their own stuff
//

//
// our modules to hot load, functions should be in the module and should be set to its module context
//
// todo : this is a generic context, need to create custom contexts for each module that we create, then use a void* to pass that context via on_load()
//
struct module_context_t
{
	// the name of our module
	const char* name;

	// versioning for the module
	uint8_t		major;
	uint8_t		minor;

	// todo : dont really need the desc and author, just for testing

	// module description
	const char* desc;

	// module author
	const char* author;

	// whether this context was successfully loaded and setup
	bool loaded = false;

	// pointers to our modules functions
	// todo : add arguments
	bool (*CTX_INIT_FN)(engine_context_t*)	= nullptr;
	void (*CTX_UPDATE_FN)()					= nullptr;
	void (*CTX_INPUT_FN)()					= nullptr;
	bool (*CTX_UNLOAD_FN)()					= nullptr;

	// optional funcs
	void (*CTX_RELOAD_FN)() = nullptr;

	// todo : add more functions as we create more hooks for functions


// engine only stuff
#ifndef HOT_MOD

	//
	// wrapper functions that check if our module funcs are valid before we run them
	// only want thse functions in our engine, hence the #ifndef
	//

	bool on_load(engine_context_t* _engine)
	{
		if (CTX_INIT_FN)
			return CTX_INIT_FN(_engine);
		DO_ONCE(printerror(std::format("no {} for '{}'", TO_STRING(CTX_INIT_FN), name)));
		return false;
	}

	void on_update()
	{
		if (CTX_UPDATE_FN)
			CTX_UPDATE_FN();
		else
			DO_ONCE(printerror(std::format("no {} for '{}'", TO_STRING(CTX_UPDATE_FN), name)));
	}

	void on_input()
	{
		if (CTX_INPUT_FN)
			CTX_INPUT_FN();
		else
			DO_ONCE(printerror(std::format("no {} for '{}'", TO_STRING(CTX_INPUT_FN), name)));
	}

	void on_unload()
	{
		if (CTX_UNLOAD_FN)
			CTX_UNLOAD_FN();
		else
			DO_ONCE(printerror(std::format("no {} for '{}'", TO_STRING(CTX_UNLOAD_FN), name)));
	}

	void on_reload()
	{
		if (CTX_RELOAD_FN)
			CTX_RELOAD_FN();
		//else
			//DO_ONCE(printerror(std::format("no {} for '{}'", TO_STRING(CTX_RELOAD_FN), name)));
	}

	//
	// prints the info for this module
	//
	void print_info() const
	{
		if (!loaded)
			printerret(;, "module isnt loaded");

		printdebug("info for module '" << name << "'");

		if (desc)						printdebug("  desc    : " << desc);
		if (author)						printdebug("  author  : " << author);
		if (major > 0 || minor > 0)		printdebug("  version : " << (int)major << "." << (int)minor);
	}

#endif
};


//
// custom context test
//
// todo : need to test
//
struct mod_test_ctx_t
{
	bool (*on_load_fn)(engine_context_t*, std::string) = nullptr;
	void (*on_update_fn)(float) = nullptr;
};
