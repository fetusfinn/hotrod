//
//	input.h | Finn Le Var
//
#pragma once

#include "keycode.h"    // includes glfw

//
// input state enum that works with both key:: and mouse::
//
enum keystate_t
{
    KEY_PRESSED,     // key/button pressed this tick
    KEY_RELEASED,    // key/button released this tick
    KEY_HELD,        // key held down for multiple ticks
};

//
// keyboard input
//
namespace key
{
    void callback(GLFWwindow* _window, int _key, int _scan_code, int _action, int _mods);
    bool check(keycode_t _key, keystate_t _state);
    void reset();

    // wrappers for the different key states
    inline bool pressed(keycode_t _key)     { return check(_key, KEY_PRESSED);  }
    inline bool released(keycode_t _key)    { return check(_key, KEY_RELEASED); }
    inline bool held(keycode_t _key)        { return check(_key, KEY_HELD);     }

    // 
    // check function for multiple keys in the same state
    // todo : probs dont need this bc we'd more likely need to check keys of different states like HELD + PRESSED
    //
    template<typename... key_t>
    bool check(keystate_t _state, key_t... _keys)
    {
        return (... && check(_keys, _state));
    }
}

//
// flags to check whether the mouse or scroll wheel have changeed this tick
//
// doing it this way so that we can combine the result into one return value
// in case both axes where changed, which makes it easier to check
//
// how to check if both are active at once : ((flags & mask) == mask)  OR  util::has_flags()
//
// todo : possibly rename to something like input_flags_t
//
enum mouse_input_changed_t : uint8_t
{
    X_CHANGED = 1 << 0,
    Y_CHANGED = 1 << 1,

    // mask for if both have changed
    XY_CHANGED = X_CHANGED | Y_CHANGED,
};

//
// simple bounding box struct
// todo : maybe move this
//
struct box_t
{
    float x, y, w, h;
};

//
// mouse input
//
namespace mouse
{
    // cursor position
    extern double x, y;

    // cursor position last click
    extern double click_x, click_y;;

    // cursor pos delta
    extern double xdelta, ydelta;

    // whether our cursor is visible
    extern bool visible;

    // whether we're printing the mouse coords
    extern bool print;

    //
    // returns the absolute value of the given value
    //
    inline double abs(double _val)
    {
        if (_val < 0)
            return _val * -1;
        return _val;
    }

    //
    // checks if two doubles are equal to a certain precision
    //
    inline bool equals(double _first, double _second, double _precision = 0.001) noexcept
    {
        return abs(_first - _second) < _precision;
    }

    // scroll wheel
    namespace wheel
    {
        // how much the scroll wheel's changed, todo : maybe rename
        extern double x, y;

        //
        // returns a value that contains flags indicating whether the scoll wheel has changed
        //
        // doing it this way bc im using doubles which are floating point, so using
        // the equality operator is unreliable due to floating point precision errors,
        // so im having to use math::equals instead, and this just wraps all that
        //
        inline uint16_t changed()
        {
            uint16_t flags = 0;

            // if the values are non zero then theyve changed

            if (!equals(mouse::wheel::x, 0))
                flags |= X_CHANGED;

            // dont want to use else if in case there was input on both axes

            if (!equals(mouse::wheel::y, 0))
                flags |= Y_CHANGED;

            return flags;
        }
    }

    void button_fn(GLFWwindow* _window, int _button, int _action, int _mods);
    void position_fn(GLFWwindow* _window, double _x, double _y);
    void scroll_fn(GLFWwindow* _window, double _xoffset, double _yoffset);

    bool check(mouse_button_t _button, keystate_t _state);
    void reset();

    //
    // returns a value that contains flags indicating whether the cursor pos has changed
    //
    // check mouse::wheel::changed() for the reasoning of doing it like this
    //
    inline uint16_t moved()
    {
        uint16_t flags = 0;

        // if the values are non zero then theyve changed

        if (!equals(mouse::xdelta, 0))
            flags |= X_CHANGED;

        // not using else if in case there was input on both axes

        if (!equals(mouse::ydelta, 0))
            flags |= Y_CHANGED;

        return flags;
    }

    //
    // checks if the mouse is in the given area
    // todo : this could be moved to util::
    //
    inline bool in_area(const box_t& _box)
    {
        return (x >= _box.x && x <= _box.x + _box.w) && (y >= _box.y && y <= _box.y + _box.h);
    }
}


/* unused
 * copy of mouse_input_changed_t but shorter names
 * bit too ambigious
    enum axis_changed_t : int
    {
        AXIS_X = 1 << 0,
        AXIS_Y = 1 << 1,
    };
*/