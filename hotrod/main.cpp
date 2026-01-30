//
//	hotrod - hot loading lib
//
#include <thread>
#include <chrono>

#include "dll.h"
#include "dll_manager.h"
#include "test.h"
#include "shared/subsystem.h"

//
//	todos
//
//	- add config file for reload interval, module paths, etc.
//	- add input handling for manual reload trigger
//

// so that we can type things like 10s or 50ms
using namespace std::chrono_literals;

//
// our engine and subsystem contexts
//
namespace ctx
{
	// test context
	sub_test_ctx_t m_test =
	{
		.dump  = [](module_context_t* _mod) { test::dump(_mod);  },
		.print = [](const std::string& _str) { test::print(_str); },
	};

	// all of our sub systems
	subsystem_info_t m_subsystems[] =
	{
		//{ "pool", &m_pool },
		{.name = "test", .data = &m_test },
	};
}

// only the engine context is exposed to the engine, as it will be using the subsystems
// directly, rather than accessing them via the contexts, the contexts are only for the
// modules to use

//
// our engine context for us to pass to modules
//
engine_context_t g_engine =
{
	.subsystems = ctx::m_subsystems,
	.subsystem_count = std::size(ctx::m_subsystems)
};


//
// main entry point
//
int main()
{
    printmsg("hotrod starting...");

    // initialise the dll manager with paths to look for dlls in
    g_dll.init({"."});

    // automatically find and load all dlls in our paths
    size_t loaded = g_dll.find_and_load();

    // todo : can remove this, dont really need this, since we want it watching until a dll appears
    ASSERT(loaded == 0, "failed to find and load a dll, exitting...");

    printdebug("starting main loop...");

    // whether we're running
    bool running = true;

    // current tick of our program
    int ticks = 0;

    // max number of ticks before we exit
    // will kill the program if set to a negative number
    constexpr int max_ticks = 5;

    // how often to look for new dlls, in ticks
    constexpr int search_delay = 1;

    // how long to sleep at the end of a tick
    constexpr std::chrono::duration<long long> sleep_dur = 2s;

    // while we're running
    while (running)
	{
        // look for new dlls every few ticks
        if (ticks % search_delay == 0)
        {
            printdebug("checking for new dlls...");

            // check for modules and get how many were loaded, if any
            size_t count = g_dll.find_and_load();

            if (count > 0)
                printdebug(count << " new module(s) found and loaded");
        }

        // check and reload any modified dlls
        size_t reloaded = g_dll.reload_modified();

        if (reloaded > 0)
            printdebug(reloaded << " module(s) reloaded");

        // update all loaded modules
        g_dll.update_all();

        // sleep for our set duration
        std::this_thread::sleep_for(sleep_dur);

        // increase our counter
        ticks++;

        ASSERT(ticks >= max_ticks, "finished running, killing...");

        // check if we should stop, used for debugging
        // if we set max_ticks to a negative then this will alway trigger
        if (max_ticks > 0 && ticks >= max_ticks)
        {
            running = false;
        }
    }

    printdebug("finishing...");

    // dump manager state before shutdown
    g_dll.dump();

    // unload all dlls (this happens automatically in dll_manager destructor, but explicit is nice)
    g_dll.unload_all();

    // cleanup subsystems
    g_subsystem.cleanup();

    printmsg("hotrod shutdown complete");

    return 0;
}
