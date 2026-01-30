//
//	rod module - hot-loadable DLL with ImGui support
//
#define HOT_DEBUG			// enable debug build for this module
#define HOT_MOD		"imgui"	// tell the project we're a module, also the name of this module

#include <thread>
#include <utility>
#include <functional>

#include "shared/context.h"
#include "shared/macros.h"
#include "shared/print.h"
#include "shared/subsystem.h"

// ImGui for creating windows
#include "imgui/imgui.h"

// for functions that we're exporting to the main program
#define HOT_EXPORT extern "C" __declspec(dllexport)

//
// static vars
//
namespace
{
	// our engine context
	engine_context_t* g_engine = nullptr;

	// this modules context
	module_context_t* g_mod = nullptr;

	// imgui subsystem
	sub_imgui_ctx_t* g_imgui = nullptr;

	// demo counter to show module is working
	int g_counter = 0;
}

//
// our module's imgui window
//
static void render_module_window()
{
	ImGui::Begin("Rod Module");

	ImGui::Text("Hello from hot-loaded module!");
	ImGui::Separator();

	ImGui::Text("Counter: %d", g_counter);

	if (ImGui::Button("Increment"))
		g_counter++;

	if (ImGui::Button("Reset"))
		g_counter = 0;

	//ImGui::Separator();
	//ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Modify this file and rebuild to see hot-reload!");

	ImGui::End();
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

	// initialise our subsys manager and parse the subsystems in our engine ctx
	g_subsystem.init(_ctx);

	// get the imgui subsystem
	g_imgui = g_subsystem.find<sub_imgui_ctx_t>(SUB_IMGUI);

	if (g_imgui)
	{
		// set imgui context before we do anything with imgui
		if (g_imgui->ctx)
			ImGui::SetCurrentContext(static_cast<ImGuiContext*>(g_imgui->ctx));

		if (g_imgui->register_fn)
		{
			printmsg("registering imgui window");

			g_imgui->register_fn(render_module_window);
		}
	}
	else
	{
		printerror("failed to get imgui subsystem");
	}



	printmsg("module " << HOT_MOD << " initialised");

	return g_engine != nullptr;
}

//
// called on input
//
void MOD_INPUT_FN()
{
	// handle input if needed
}

//
// called on update
//
void MOD_UPDATE_FN()
{
	// update logic if needed

	return;

	printmsg("ummm");
	printmsg("uhhhhhh");
	printmsg("uhhhmmmmm");

	return;

	printmsg("dllz NUTZ");
	printmsg("HAAAA");
	printmsg("GOTTEEM");
}

//
// called when our module is unloaded from the engine
// returns true if everything successfully shutdown
//
bool MOD_UNLOAD_FN()
{
	printmsg("module unloading");

	// clear our imgui functions before unloading
	if (g_imgui && g_imgui->clear_fns)
		g_imgui->clear_fns();

	g_mod	 = nullptr;
	g_engine = nullptr;
	g_imgui  = nullptr;

	// succesfully unloaded
	return true;
}

//
// called when this module needs to internally reload
//
void MOD_RELOAD_FN()
{
	printmsg("module reloading");

	// re-register our imgui window after reload
	if (g_imgui)
	{
		// set imgui context again after reload
		if (g_imgui->ctx)
			ImGui::SetCurrentContext(static_cast<ImGuiContext*>(g_imgui->ctx));

		if (g_imgui->register_fn)
			g_imgui->register_fn(render_module_window);
	}
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
	_mod->desc		= "imgui test";
	_mod->major		= 1;
	_mod->minor		= 0;

	// store our module's func in the context so that the engine can access and use them
	_mod->CTX_INIT_FN	= &MOD_INIT_FN;
	_mod->CTX_INPUT_FN  = &MOD_INPUT_FN;
	_mod->CTX_UPDATE_FN = &MOD_UPDATE_FN;
	_mod->CTX_UNLOAD_FN = &MOD_UNLOAD_FN;
	_mod->CTX_RELOAD_FN	= &MOD_RELOAD_FN;

	// if on_init, on_input, on_update, and on_unload were set then the module is considered loaded
	_mod->loaded = _mod->CTX_INIT_FN && _mod->CTX_INPUT_FN && _mod->CTX_UPDATE_FN && _mod->CTX_UNLOAD_FN;

	// store our mod
	g_mod = _mod;

	printfunc("context setup");
}
