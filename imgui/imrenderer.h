/*
 *  imrenderer.h
 */
#pragma once

#include "imctx.h"

#include "shared/macros.h"

//
// our different fonts
//
namespace font
{
    extern ImFont *big, *small;
}

//
// custom renderer using imgui
//
class im_renderer_t
{
private:

    // whether our fonts have been initialised
    static bool m_fonts_init;

    // our draw list
    ImDrawList* m_draw = nullptr;

    // whether our render window is open
    bool m_open = true;

    // the style that we applied before we started rendering the
    // current frame using our imrenderer
    ImGuiStyle m_last_style = {};
    
private:
    
    ImFont* create_font(const char* font_path, float font_size) const;
    
public:

     im_renderer_t() = default;
    ~im_renderer_t() = default;

    void start(bool _focus = true);
    void finish();

    void init_fonts() const;
    
    void draw_box(float x, float y, float w, float h, ImColor col) const;
    void draw_box_filled(float x, float y, float w, float h, ImColor col) const;
    void draw_box_outlined(float x, float y, float w, float h, float thickness, ImColor col, ImColor border) const;
    void draw_box_gradient(float x, float y, float w, float h, ImColor col1, ImColor col2) const;
    // void draw_box_3d(Vector origin, Vector min, Vector max, ImColor col);
    
    void draw_line(float x, float y, float xx, float yy, ImColor col) const;
    void draw_string(float x, float y, const char* str, ImFont* font, ImColor col, bool centered = false) const;
    void draw_polygon(ImVec2* verts, ImColor col) const;
    void draw_circle(ImVec2 center, float radius, ImColor col) const;
    void draw_circle_filled(ImVec2 center, float radius, ImColor col) const;
    
    ImVec2  get_text_size(const char* str) const;
    bool    in_area(float x, float y, float w, float h, bool draw = false) const;

    // make this class a singleton since we only need a single instance
    // todo idea : could possibly remove this so that we could create multiple instances if we find a need
    MAKE_SINGLETON(im_renderer_t)
};

// create a ref to our instance called g_render
MAKE_SINGLETON_ALIAS(im_renderer_t, render);