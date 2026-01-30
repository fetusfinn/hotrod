//      renderer.cpp
//
//
//
//
#include "imrenderer.h"

#include "glctx.h"
#include "imctx.h"
#include "print.h"

// whether we've initialised our fonts
bool im_renderer_t::m_fonts_init = false;

/****************************** window ******************************/

//
// creates a new imgui window for us to draw everything to
//
void im_renderer_t::start(bool _focus)
{
    // initialise our fonts if we havent already
    if(!im_renderer_t::m_fonts_init)
        this->init_fonts();

    // get the style
    ImGuiStyle& style = ImGui::GetStyle();

    // store the style so we can restore it after
    m_last_style = style;

    // set up our imgui window to be the size of our window
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ (float)g_glctx.m_window_w, (float)g_glctx.m_window_h });
    ImGui::SetNextWindowBgAlpha(0.f);

    // remove the window border
    style.WindowBorderSize = 0.f;

    // todo : maybe add the ability to pass a lambda that sets the style and window flags
    // example :
    auto stylise = [](ImGuiStyle& _style)
    {
        // _style.ItemSpacing = ImVec2{ 10.f, 10.f };
        // ...
    };

    // the flags for our window so it's just blank
    int flags = ImGuiWindowFlags_NoTitleBar     | ImGuiWindowFlags_NoResize |
		        ImGuiWindowFlags_NoScrollbar    | ImGuiWindowFlags_NoInputs |
		        ImGuiWindowFlags_NoSavedSettings;

    // if we dont want our window to be focused on, then also apply these flags
    if (!_focus)
        flags |= ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;

    // create our render window
    ImGui::Begin("imrenderer window", &m_open, flags);

    // get the draw list for this window
    this->m_draw = ImGui::GetWindowDrawList();
}

//
// ends our imgui window frame
//
void im_renderer_t::finish()
{
    ImGui::End();

    // restore the style we were using before we created our window
    ImGui::GetStyle() = m_last_style;
}

/****************************** drawings ******************************/

//
// draws a line between the given points
//
void im_renderer_t::draw_line(float x, float y, float xx, float yy, ImColor col) const
{
    m_draw->AddLine({ x, y }, { xx, yy }, col);
}

//
// draws a hollow rect
//
void im_renderer_t::draw_box(float x, float y, float w, float h, ImColor col) const
{
    m_draw->AddRect({x, y}, {x + w, y + h}, col);
}

//
// draws a filled rect
//
void im_renderer_t::draw_box_filled(float x, float y, float w, float h, ImColor col) const
{
    m_draw->AddRectFilled({x, y}, {x + w, y + h}, col);
}

//
// draws a rect with an outline
//
void im_renderer_t::draw_box_outlined(float x, float y, float w, float h, float thickness, ImColor col, ImColor border) const
{
    m_draw->AddRectFilled({x - thickness, y - thickness}, {x + w + thickness, y + h + thickness}, border);
    m_draw->AddRectFilled({x, y}, {x + w, y + h}, col);
}

//
// draws a rect that gradients from one coloue to another, top to bottom
//
void im_renderer_t::draw_box_gradient(float x, float y, float w, float h, ImColor col1, ImColor col2) const
{
    m_draw->AddRectFilledMultiColor({x, y}, {x + w, y + h}, col1, col1, col2, col2);
}

/* unused
//  draw_box_3d
//  Draws a 3d box around a point
//
 void im_renderer_t::draw_box_3d(Vector origin, Vector min, Vector max, ImColor col)
 {
	 min += origin;
	 max += origin;
	 
	 Vector points[] =
	 {
		 Vector(min.x, min.y, min.z),
		 Vector(min.x, max.y, min.z),
		 Vector(max.x, max.y, min.z),
		 Vector(max.x, min.y, min.z),
		 Vector(min.x, min.y, max.z),
		 Vector(min.x, max.y, max.z),
		 Vector(max.x, max.y, max.z),
		 Vector(max.x, min.y, max.z)
	 };
	 
	 int edges[12][2] =
	 {
		 { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
		 { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 },
		 { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
	 };
	 
	 for (auto it : edges)
	 {
		 Vector p1, p2;

		 if (!world_to_screen(points[it[0]], p1) || !world_to_screen(points[it[1]], p2))
			return;
		 
		 this->draw_line(p1.x, p1.y, p2.x, p2.y, col);
	 }
 }
*/

//
// draws a polygon with given verticies
//
void im_renderer_t::draw_polygon(ImVec2* verts, ImColor col) const
{
    m_draw->AddConvexPolyFilled(verts, IM_ARRAYSIZE(verts), col);
}

//
// draws a hollow circle
//
void im_renderer_t::draw_circle(ImVec2 center, float radius, ImColor col) const
{
    m_draw->AddCircle(center, radius, col);
}

//
// draws a filled circle
//
void im_renderer_t::draw_circle_filled(ImVec2 center, float radius, ImColor col) const
{
    m_draw->AddCircleFilled(center, radius, col);
}

//
// draws a string
//
void im_renderer_t::draw_string(float x, float y, const char* str, ImFont* font, ImColor col, bool centered) const
{
    // do we want to center our text?
    if (centered)
    {
        // if so then get the text size and offset it
        auto text_size = get_text_size(str);

        x -= (text_size.x / 2);
        y -= (text_size.y / 2);
    }

    ImGui::PushFont(font);
	m_draw->AddText({ x, y }, col, str);
    ImGui::PopFont();
}

/****************************** utils ******************************/

//
// returns the given strings width and height
//
ImVec2 im_renderer_t::get_text_size(const char* str) const
{
    return ImGui::CalcTextSize(str);
}

//
// returns true if the mouse is in the given area
//
bool im_renderer_t::in_area(float x, float y, float w, float h, bool draw) const
{
    // get the mouse pos
    auto mouse = ImGui::GetIO().MousePos;

    // check if we're in the given quad
    bool in = (mouse.x > x && mouse.y > y) && (mouse.x < x + w && mouse.y < y + h);

    // do we want to draw the area that we're checking? used for debugging
    if(draw)
        this->draw_box(x, y, w, h, (in ? ImColor(0, 0, 255) : ImColor(255, 0, 0)));
    
    return in;
}

/****************************** fonts ******************************/

//
// our font objects
//
namespace font
{
    ImFont* big;    // 13px
    ImFont* small;  // 10px
}

//
// loads the font at the given path, and creates it at the given size
//
ImFont* im_renderer_t::create_font(const char* font_path, float font_size) const
{
    // setup the font config for our new font
    ImFontConfig font_conf;

    font_conf.OversampleH = 1;
    font_conf.OversampleV = 1;
    font_conf.PixelSnapH = true;

    printdebug("created font : " << font_path);

    // create and return our font
    return ImGui::GetIO().Fonts->AddFontFromFileTTF(font_path, font_size, &font_conf);
}

//
// sets up our fonts
//
void im_renderer_t::init_fonts() const
{
    // check if we've already updated our fonts
    if (im_renderer_t::m_fonts_init)
        return;

    return;

    ImFontConfig cfg;

    cfg.SizePixels = 13.f;

    font::big = im::GetIO().Fonts->AddFontDefault(&cfg);

    cfg.SizePixels = 12.f;

    font::small = im::GetIO().Fonts->AddFontDefault(&cfg);

    // finished creating fonts, set to true
    im_renderer_t::m_fonts_init = true;
}

/*  for mac

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>

//
//  init_fonts
//  Initialises fonts
//
void im_renderer_t::init_fonts()
{
    if(fonts_init)
        return;

    auto font = im::GetFont();

    passwd* pwd = getpwuid(getuid());
    std::string fonts_dir = std::string(pwd->pw_dir) + "/Library/Fonts/";
    
    Fonts::big  = create_font(std::string(fonts_dir + "ProggyClean.ttf").c_str(), 13.f);
    Fonts::small= create_font(std::string(fonts_dir + "ProggyTiny.ttf").c_str(),  10.f);
    
    fonts_init = true;
}
*/