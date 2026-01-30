//
//	imctx.cpp | Finn Le Var
//
#include "imctx.h"

// imgui backends
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glctx.h"
#include "input.h"
#include "shared/print.h"

//
// static
//
namespace
{
	// what version of glsl we're using
	const char* m_glsl_version = "#version 460 core";
}

// redefine printmsg macro for this file using a different prefix
#define printmsg(_msg) printprefix(_msg, "imctx")

//
// initialises our imgui context and sets up our renderer backends and style
//
void imcontext_t::init()
{
	// shouldnt happen but just to be safe
	if (!g_glctx)
		return;

	// setup imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// setup platform/renderer backends

	// setup glfw backend
	ImGui_ImplGlfw_InitForOpenGL(g_glctx.m_window, true);

	// setup opengl backend
	ImGui_ImplOpenGL3_Init(m_glsl_version);

	// configure and stylise our imgui instance
	configure();

	// set initialise flag
	m_init = true;
}

//
// configures our imgui IO and sets up our default style
//
void imcontext_t::configure() const
{
	// todo : maybe move io stuff into its own func

	// get our io, this is the config/IO between our app and imgui
	ImGuiIO& io = ImGui::GetIO();

	// enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;




	// stylise

	// setup style
	ImGui::StyleColorsDark();

	// setup scaling
	ImGuiStyle& style = ImGui::GetStyle();

	// set initial font scale
	style.FontScaleDpi = g_glctx.m_monitor_scale;

	// bake a fixed style scale
	style.ScaleAllSizes(g_glctx.m_monitor_scale);
}

//
// shutsdown imgui
//
void imcontext_t::shutdown() const
{
	// shutdown our backends

	// shutdown opengl
	ImGui_ImplOpenGL3_Shutdown();

	// shutdown glfw
	ImGui_ImplGlfw_Shutdown();

	// destroy our context
	ImGui::DestroyContext();

	printmsg("successfully shut down");
}

//
// registers a new render func
//
void imcontext_t::registerfn(const imrenderfn_t& _fn)
{
	m_fns.push_back(_fn);

	printmsg("registered new render fn");
}

//
// clears all registered render functions
//
void imcontext_t::clearfns()
{
	m_fns.clear();

	printmsg("cleared all render fns");
}

//
// starts a new imgui frame
//
void imcontext_t::new_frame()
{
	// dont need to run this if we've already created a new frame
	if (m_have_frame)
		return;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// set new frame flag
	m_have_frame = true;
}

//
// draws our imgui data
//
void imcontext_t::render() const
{
	// if we havent created an imgui frame this tick then dont do anything
	if (!m_have_frame)
		return;

	// prepare our draw data
	ImGui::Render();

	// draw our imgui data
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//
// called when a frame starts
//
void imcontext_t::frame_start()
{
	// reset values
	m_have_frame	= false;
	m_rendered		= false;
}

//
// called when the frame ends
//
void imcontext_t::frame_end()
{
	// just to shut up warnings
	m_rendered = false;
}

//
// build's our gui
//
void imcontext_t::build()
{
	// create our new frame
	new_frame();

	// run all of our render functions
	for (imrenderfn_t& renderfn : m_fns)
		renderfn();
}

//
// sleeps our program for the given duration in milliseconds
//
void imcontext_t::sleep(int _ms) const
{
	// this is just a wrapper for Sleep() from windows.h
	ImGui_ImplGlfw_Sleep(_ms);
}
