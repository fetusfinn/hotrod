//
//	dll.h | Finn Le Var
//
#pragma once

#include <windows.h>
#include <unordered_map>

#include "print.h"

// for functions and variables that we're reading from the dll
#define HOT_LOAD extern "C"

// so that we can load functions of different return types that have 
// the same base function sig that we're looking for
template<typename type_t>
using dllfntype_t = type_t(*)();

//
// dll handle container
// todo : check if dll has already been loaded
//
struct dll_t
{
    // our dll's handle
    HMODULE m_handle = nullptr;

    // the path to our dll
    std::string m_path = "";

    // the last time our dll was updated
    std::filesystem::file_time_type m_last_update;

    dll_t(const std::string& _path = "")
    {
	    load(_path);

        m_last_update = std::filesystem::last_write_time(m_path);
    }

	~dll_t()
    {
	    unload();
    }

    //
    // loads a dll int memory from the given path
    //
    bool load(const std::string& _path)
    {
        if (m_handle)
            printerret("dll already loaded", false);

        // no path given, cant load nothing
        if (_path.empty())
            return false;

        // store the given path
        m_path = _path;

        // try load our lib using the winapi
        m_handle = LoadLibraryA(_path.c_str());

        // check if we failed, error and return if so
        if (!m_handle)
            printerret("failed to load dll", false);

        return true;
    }

    //
    // checks if its dll has been edited and reloads if so
    //
    void reload()
    {
        // check the last time our dll was updated
        auto update_time = std::filesystem::last_write_time(m_path);

        // has it changed?
        if (update_time != m_last_update)
        {
            // if so, then reload our dll

            printdebug("reloading...");

            // unload
            unload();

            // then reload
            load(m_path);

            // update last load
            m_last_update = update_time;
        }
    }

    //
    // unloads our dll
    //
    void unload()
    {
        if (!m_handle)
            return;

        FreeLibrary(m_handle);

        m_handle = nullptr;

        m_path.clear();
    }

    //
    // tries to find the func with the given name in our dll and returns it
    //
    template<typename type_t>
    dllfntype_t<type_t> find_fn(const std::string& _func)
    {
        if (!m_handle)
            printerret("no dll loaded", nullptr)

        // try to find our function in our dll's memory
        auto fn = reinterpret_cast<dllfntype_t<type_t>>(GetProcAddress(m_handle, _func.c_str()));

        if (!fn)
            printerret(std::format("unable to find func '{}' in dll", _func), nullptr)

        return fn;
    }

    //
    // so that we can easily check if this instance has been loaded
    //
    bool operator !() const
    {
        return !m_handle;
    }
};
