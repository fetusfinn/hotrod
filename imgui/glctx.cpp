//
//	glctx.cpp | Finn Le Var
//
#include "glctx.h"

#include "input.h"
#include "shared/macros.h"

//
// static
//
namespace
{
	// whether we've set our callback fns, only need to set them once over all contexts
	bool m_callbacks_set = false;

	//
	// called when glfw errors
	//
	void error_callback_fn(int _code, const char* _error)
	{
		std::cerr << "[error] GLFW error " << _code << " : " << _error << "\n";
	}
}

// redefine printmsg macro for this file using a different prefix
#define printmsg(_msg) printprefix(_msg, "glctx")

//
//
//
glcontext_t::~glcontext_t()
{
	this->shutdown();
}

//
//
//
bool glcontext_t::init(uint16_t w, uint16_t h)
{
	// check we havent already initialised, this shouldnt happen but for safety
	if (m_window)
		printerret(false, "gl context has already been initialised");

	// store the given window dimensions
	m_window_w = w;
	m_window_h = h;

	// initialise GLFW
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 4);					// multi sampling

	// get monitor content scale
	float y_scale; glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &m_monitor_scale, &y_scale);

	// try create a window using GLFW
	m_window = glfwCreateWindow((int)(w * m_monitor_scale), (int)(h * m_monitor_scale), "imgui", nullptr, nullptr);

	// check if we initialised everything correctly and created our window
	if (!m_window)
	{
		printerror("failed to initialise GLFW, exitting...");

		// shutdown GLFW
		glfwTerminate();

		return false;
	}

	// change our context to our window
	glfwMakeContextCurrent(m_window);

	// try initialise GLEW
	if (glewInit() != GLEW_OK)
	{
		printerror("failed to initialise GLEW, exitting...");

		// destroy the window
		glfwDestroyWindow(m_window);

		// shutdown GLFW
		glfwTerminate();

		return false;
	}

	// set our callbacks if we havent already
	if (!m_callbacks_set)
	{
		glfwSetErrorCallback(error_callback_fn);

		glfwSetKeyCallback			(m_window, key::callback);
		glfwSetMouseButtonCallback	(m_window, mouse::button_fn);
		glfwSetCursorPosCallback	(m_window, mouse::position_fn);
		glfwSetScrollCallback		(m_window, mouse::scroll_fn);

		m_callbacks_set = false;
	}

	printmsg("initialised");

	return true;
}

//
//
//
void glcontext_t::shutdown()
{
	// destroy the window
	glfwDestroyWindow(m_window);

	// shutdown OpenGL
	glfwTerminate();

	printmsg("successfully shut down");
}

//
// called at the beginning of a frame
//
void glcontext_t::frame_start()
{
	// update the last time
	m_last_time = m_cur_time;

	// get the current time
	m_cur_time = glfwGetTime();

	// update the delta time
	m_delta = m_cur_time - m_last_time;

	// increase the time elapsed
	m_elapsed += m_delta;

	// poll all events
	glfwPollEvents();
}

//
//
//
void glcontext_t::update()
{
	// quit on esc press
	if (key::check(KEY_DELETE, KEY_PRESSED))
		glfwSetWindowShouldClose(m_window, true);


}

//
// prepares opengl to render, not drawing anything atm
// todo : maybe call this prerender
//
void glcontext_t::render()
{

	// prepare opengl frame to draw

	// get our frame buffer size
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);

	// then set the viewport to the size of our frame buffer
	glViewport(0, 0, display_w, display_h);

	// set the clear colour
	glClearColor(m_clear_colour.x * m_clear_colour.w, m_clear_colour.y * m_clear_colour.w, m_clear_colour.z * m_clear_colour.w, m_clear_colour.w);

	// and clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
}

//
// called at the end of the frame and displays everything that we've drawn this frame
//
void glcontext_t::frame_end()
{
	// show everything that we've just drawn
	glfwSwapBuffers(m_window);
}

//
// returns true if our program should keep running
//
bool glcontext_t::should_run() const
{
	return !glfwWindowShouldClose(m_window);
}

//
// returns true if the window is minimised
//
bool glcontext_t::is_minimised() const
{
	return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0;
}

//
// closes the window and ends the program
//
void glcontext_t::close() const
{
	glfwSetWindowShouldClose(m_window, true);
}
