//
//	input.cpp | Finn Le Var
//
#include "input.h"

#include <unordered_set>
#include <unordered_map>

#include "shared/macros.h"

// anon namespace to restrict the visibility of these types to just this file
// just to be safe, even tho these will most probably never be redefined
namespace
{
    // just for now
    using keymod_t = int;

    // type for the input and modifiers
    using key_input_t = std::pair<int, keymod_t>;

    // change to key_input_t, will have to change check funcs to use find_if

    // type for storing each key/button's state
    using key_set_t = std::unordered_set<int>;

    // type for when a key or button is held, int is keycode, uint is held duration in ticks
    using held_map_t = std::unordered_map<int, uint32_t>;

    // how many ticks a key must be held down for before we consider it held
    // using 3 as it's short enough to not be noticeable, and feel like things are lagging,
    // while still being long enough to distinguish from a normal press
    // todo : should use time, millisec, ~80ms is a good value
    constexpr uint8_t g_held_delay = 3;

    //
    // increases the counters in the given held ticks map, and moves
    // any keys that have been held long enough into the given held list
    //
    void increase_held_ticks(held_map_t& _held_ticks, key_set_t& _held)
    {
        if (_held_ticks.empty())
            return;

        // then iterate over and increase the duration they've been held
        for (auto& [key, ticks] : _held_ticks)
        {
            // increase tick then check if it's been held long enough
            if (++ticks > g_held_delay)
            {
                _held.insert(key);
            }
        }

        // remove the element(s) we just moved to held from m_held_ticks
        for (auto key : _held)
            _held_ticks.erase(key);
    }
}

//
// keyboard input
//
namespace key
{
    // anon namespace to restrict the visibility of our containers
    namespace
    {
        // list of all keys that were pressed/released this tick
        key_set_t m_pressed;
        key_set_t m_released;

        // list of all keys that are currently pressed
        key_set_t m_held;

        // list of keys and how long they've been held for
        held_map_t m_held_ticks;
    }

    //
    // key callback function for GLFW
    //
    void callback(GLFWwindow* _window, int _key, int _scan_code, int _action, int _mods)
    {
        if (_action == GLFW_PRESS)
        {
            // add to "just pressed" and "held" lists
            m_pressed.insert(_key);

            // add to our delayed list
            m_held_ticks.insert({ _key, 0 });
        }
        else if (_action == GLFW_RELEASE)
        {
            // remove from the held lists
            m_held.erase(_key);
            m_held_ticks.erase(_key);

            // add to "just released" list
            m_released.insert(_key);
        }
        else if (_action == GLFW_REPEAT)
        {
            // repeat is called after a key is held, sends a GLFW_PRESS then GLFW_REPEAT if still held

            // the built-in delay is too long
        }
    }

    //
    // returns true if the given key is in the given state
    //
    bool check(keycode_t _key, keystate_t _state)
    {
        switch (_state)
        {
        case KEY_PRESSED:   return m_pressed.contains(_key);
        case KEY_HELD:      return m_held.contains(_key);
        case KEY_RELEASED:  return m_released.contains(_key);
        }

        return false;
    }

    //
    // resets stored key states
    //
    void reset()
    {
        m_pressed.clear();
        m_released.clear();

        // not clearing held since we want it to stay stored until the user
        // releases it, not at the end of the tick

        // increase the held ticks
        increase_held_ticks(m_held_ticks, m_held);
    }
}

//
// mouse input
// todo : could use vec2s for all the x and y stuff
//
namespace mouse
{
    // blank namespace to restrict the visibility of our containers
    namespace
    {
        // list of all mouse buttons that were pressed/released this tick
        key_set_t m_pressed;
        key_set_t m_released;

        // list of all mouse buttons that are currently pressed
        key_set_t m_held;

        // list of keys and how long they've been held for
        held_map_t m_held_ticks;
    }

    namespace wheel
    {
        // the scroll offset this tick
        double x, y;
    }

    // our mouse cursor's position
    double x, y;

    // the mouse position when it was clicked
    double click_x, click_y;

    // cursor pos delta
    double xdelta = 0, ydelta = 0;

    // whether our cursor is visible
    bool visible = true;

    // whether we're printing the mouse coords
    bool print = false;

    //
    // mouse button callback
    //
    void button_fn(GLFWwindow* _window, int _button, int _action, int _mods)
    {
        if (_action == GLFW_PRESS)
        {
            // add to "just pressed" and "held" lists
            m_pressed.insert(_button);

            // store the click position for left and right
            if (_button == MOUSE_LEFT || _button == MOUSE_RIGHT)
            {
                click_x = x;
                click_y = y;
            }

            // add to our delayed list
            m_held_ticks.insert({ _button, 0 });
        }
        else if (_action == GLFW_RELEASE)
        {
            // remove from the held list and add to "just released" list
            m_held.erase(_button);
            m_held_ticks.erase(_button);

            m_released.insert(_button);
        }
        else if (_action == GLFW_REPEAT)
        {
            // repeat is called after a key is held, sends a GLFW_PRESS then GLFW_REPEAT if still held
            // built in delay is too slow
        }
    }

    //
    // cursor position callback 
    //
    void position_fn(GLFWwindow* _window, double _x, double _y)
    {
        // temp
        double oldx = x, oldy = y;

        // store the new position
        x = _x;
        y = _y;

        // caclulate delta
        xdelta = x - oldx;
        ydelta = y - oldy;

        // do we want to print the position data
        if (print)
            printdebug(std::format("mouse position : {:.3f} , {:.3f}   |   mouse moved by : {:.3f} , {:.3f}", x, y, xdelta, ydelta));
    }

    //
    // scroll wheel callback
    //
    void scroll_fn(GLFWwindow* _window, double _xoffset, double _yoffset)
    {
        // store the changed offset(s)
        wheel::x = _xoffset;
        wheel::y = _yoffset;
    }

    //
    // check mouse states
    //
    bool check(mouse_button_t _button, keystate_t _state)
    {
        switch (_state)
        {
        case KEY_PRESSED:  return m_pressed.contains(_button);
        case KEY_HELD:     return m_held.contains(_button);
        case KEY_RELEASED: return m_released.contains(_button);
        }

        return false;
    }

    //
    // resets stored mouse states
    //
    void reset()
    {
        m_pressed.clear();
        m_released.clear();

        wheel::x = 0; wheel::y = 0;

        // reset since this is the position when clicked
        // click_x = 0; click_y = 0;

        // reset in case the mouse doesnt move next tick
        xdelta = 0; ydelta = 0;

        // increase the held ticks
        increase_held_ticks(m_held_ticks, m_held);
    }
}
