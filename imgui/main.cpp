//
//	main.cpp | Finn Le Var
//
#include "glctx.h"
#include "imctx.h"
#include "input.h"
#include "gui.h"

#include "shared/macros.h"
#include "shared/context.h"
#include "hotrod/dllman.h"

// 
// todo 
// 
// - https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html
// 
// - create a test window with examples that i could and would use
// 
// - look at docking branch on github
// 
// project ideas
// 
// - create a system that lets you load in an asset from a path
//	 - could use this with the file archiver for testing
//
// - integrate into graphics project
//   - feature that lets you click on a mesh in the scene then it opens a window with info about said mesh
//

namespace
{
	//
	// registers our gui funcs in our context
	//
	void register_gui()
	{
		g_imctx.registerfn(gui::hotrod_window);
	}

	namespace ctx
	{
		//
		// imgui subsystem context instance
		//
		sub_imgui_ctx_t imgui = {};
	}

	//
	// subsystem array
	//
	subsystem_info_t g_subsystems[] =
	{
		{ "SUB_IMGUI", &ctx::imgui }
	};
}

//
// our engine context definition
//
engine_context_t g_engine = { g_subsystems, std::size(g_subsystems) };


//
// todo : put dll searching in its own thread, separate from the main thread, so that main can still tick and update things
//
int main()
{
	// used to calculate our window width based off our height
	// for a well proportioned window
	float scale = 1280.f / 800.f;

	// our window height
	uint16_t h = 800 / 2;

	// initialise our opengl context
	if (!g_glctx.init(h * scale, h))
		printerret(1, "failed to init gl context");

	// initialise our imgui context
	g_imctx.init();

	// register our gui
	register_gui();

	// setup imgui subsystem function pointers
	ctx::imgui =
	{
		.register_fn = [](const imrenderfn_t& _fn)  { printmsg("registerfn() call"); g_imctx.registerfn(_fn); },
		.clear_fns	 = []()							{ g_imctx.clearfns(); },
		.ctx		 = ImGui::GetCurrentContext(),	// pass our imgui context to modules
	};

	// setup engine context
	g_engine.subsystems		 = g_subsystems;
	g_engine.subsystem_count = 1;

	// initialize DLL manager and search for modules
	g_dll.init({ "mods" });
	g_dll.find_and_load();

	printmsg("engine initialized");

	// main loop
	while (g_glctx.should_run())
	{
		// start our frame
		g_glctx.frame_start();
		g_imctx.frame_start();

		// check if minimised
		if (g_glctx.is_minimised())
		{
			// if we are then sleep and skip everything
			g_imctx.sleep(10);
			continue;
		}

		// check for modified DLLs and reload
		g_dll.reload_modified();

		// update our gl context
		g_glctx.update();

		// build all of our imgui windows
		g_imctx.build();

		// prepare our gl context to render
		g_glctx.render();

		// render our imgui context
		g_imctx.render();

		// end our frame
		g_imctx.frame_end();
		g_glctx.frame_end();

		// reset our input values
		key::reset();
		mouse::reset();
	}

	// cleanup
	g_imctx.shutdown();
	g_glctx.shutdown();

	return 0;
}
