//
//	glctx.h | Finn Le Var
//
#pragma once

#include "shared/macros.h"

#include <glew.h>
#include <glfw3.h>

#include <glm/vec4.hpp>

//
// context for our opengl layer
// todo : ability to create more windows
//
class glcontext_t
{
public:

	// our window
	GLFWwindow* m_window = nullptr;

	// window size
	uint16_t m_window_w = 0, m_window_h = 0;

	// the content scale of our monitor
	float m_monitor_scale = 1.f;

	// the current time
	double m_cur_time = 0;

	// curtime last tick
	double m_last_time = 0;

	// the amount of time elapsed since the app started
	double m_elapsed = 0;

	// delta time between frames
	double m_delta = 0;

	// the clear colour
	glm::vec4 m_clear_colour = glm::vec4(1.f);

public:

	 glcontext_t() = default;
	~glcontext_t();
	
	bool init(uint16_t w, uint16_t h);
	void shutdown();

	void frame_start();
	void update();
	void render();
	void frame_end();

	void close() const;

	//
	// getters
	//

	// if we have a valid window then our context is valid
	// todo : add more complex checks
	bool valid() const { return m_window != nullptr; }

	// whether we should be running
	bool should_run() const;

	// whether the window is minimised
	bool is_minimised() const;

	// bool operators 
	bool operator()() const { return  this->valid(); }
	bool operator!()  const { return !this->valid(); }

	// make this class a singleton bc we only want once instance of it ever
	MAKE_SINGLETON(glcontext_t)
};

// create a reference to our singleton instance called g_glctx
MAKE_SINGLETON_ALIAS(glcontext_t, glctx)