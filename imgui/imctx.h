//
//	imctx.h | Finn Le Var
//
#pragma once

#include "imgui/imgui.h"

#include "shared/macros.h"
#include "shared/context.h"

#include <functional>
#include <vector>

// alias to make it easier to write
namespace im = ImGui;

// forward decl
class glcontext_t;

//
// imgui context, handles all imgui related stuff
//
class imcontext_t
{
private:

	// whether we successfully initialised
	bool m_init = false;

	// whether we've called new_frame() this frame
	bool m_have_frame = false;

	// whether we've called ImGui::Render() this frame
	bool m_rendered = false;

	// all of the render functions that we've registered
	std::vector<imrenderfn_t> m_fns;

private:

	void configure() const;
	void new_frame();

public:

	 imcontext_t() = default;
	~imcontext_t() = default;

	void init();
	void shutdown() const;

	void registerfn(const imrenderfn_t& _fn);
	void clearfns();

	void build();
	void render() const;

	void frame_start();
	void frame_end();

	void sleep(int _ms) const;

	// make this class a singleton bc we only want a single instance
	MAKE_SINGLETON(imcontext_t)
};

// create a reference to our singleton instance named g_imctx
MAKE_SINGLETON_ALIAS(imcontext_t, imctx)