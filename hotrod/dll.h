//
//	dll.h | Finn Le Var
//
#pragma once

#include <windows.h>
#include <unordered_map>
#include <filesystem>
#include <chrono>

#include "util.h"
#include "shared/print.h"
#include "shared/context.h"

// for functions and variables that we're reading from the dll
#define HOT_LOAD extern "C"

// folder that stores all of our currently loaded dlls
#define CUR_FOLDER "current"

// so that we can load functions of different return types that have
// the same base function sig that we're looking for
template<typename type_t>
using dllfn_t = type_t(*)();

// for context modules
// todo : replace module_context_t* with void* so that we can use different context structs rather than a single generic struct
using module_load_fn_t = void(*)(module_context_t*);

//
// dll handle container
// note : dll lifecycle is now managed by dll_manager_t, see dll_manager.h
//
struct dll_t
{
    // our dll's handle
    HMODULE m_handle = nullptr;

    // the path to our dll
    std::string m_path;

    // the path to the copied dll that we actually load
    std::string m_copy_path;

    // the dlls filename
    std::string m_name;

    // the last time our dll was updated
    std::filesystem::file_time_type m_last_update;

    // our modules context
    module_context_t m_ctx = {};

    //
    // loads our module
    //
    dll_t(std::string _path = "") : m_path(std::move(_path))
    {
	    reload(true);
    }

    //
    // unloads our module
    //
	~dll_t()
    {
	    unload();
    }

    // 
    // returns true if the current module is loaded
    // 
    bool loaded() const
    {
        return m_ctx.loaded && m_handle;
    }

    //
    // loads the dll at the given path into memory
    //
    bool load(const std::string& _path, const std::filesystem::file_time_type& _last_update = {})
    {
        if (m_handle)
            printerret(false, "dll already loaded");

        // no path given, cant load nothing
        if (_path.empty())
            printerret(false, std::format("dll at path '{}' does not exist!", _path));

        printdebug("loading dll from '" << _path << "'");

        // path to the folder for the dll that is currently loaded
        std::filesystem::path current_dir = CUR_FOLDER;

        // create current folder if it doesn't exist
        if (!std::filesystem::exists(current_dir))
            std::filesystem::create_directory(current_dir);

        // get the file's info for us to build a unique path name using
        auto filename   = std::filesystem::path(_path).filename();
        auto stem       = filename.stem();
        auto extension  = filename.extension();
        auto timestamp  = std::chrono::system_clock::now().time_since_epoch().count();

        // build the path and store it
        m_copy_path = std::format("current/{}_{}{}", stem.string(), timestamp, extension.string());

        // copy the dll to the current folder
        try
        {
            std::filesystem::copy_file(_path, m_copy_path, std::filesystem::copy_options::overwrite_existing);
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            // clear the copy path since we didnt load anything
            m_copy_path = "";

            printerret(false, std::format("failed to copy dll '{}' : {}", stem.string(), e.what()));
        }

        printdebug("dll '" << stem.string() << "' copied to '" << m_copy_path << "'");

        // try load our copied dll
        m_handle = LoadLibraryA(m_copy_path.c_str());

        // check if we failed
        if (!m_handle)
            printerret(false, "failed to load dll");

        // successfully loaded

        // store the original path, filename, and last update time
        m_path = _path;
        m_name = stem.string();

        // if no time was given then use the current timestamp, otherwise use the given one
        // so that we dont have to call last_write_time() too often
        m_last_update = (_last_update.time_since_epoch() == std::filesystem::file_time_type::duration{}) ? std::filesystem::last_write_time(m_path) : _last_update;

        return true;
    }

    //
    // checks if its dll has been edited and reloads if so
    //
    module_context_t* reload(bool _init = false)
    {
        // the last time our dll was updated
        std::filesystem::file_time_type update_time;

        // wrapping in a try in case the dll no longer exists
        try
        {
            // check the last time our dll was updated
            update_time = std::filesystem::last_write_time(m_path);
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            printerret(nullptr, std::format("failed to stat dll '{}', {}", m_name, e.what()));
        }

        if (_init)
        {
            printdebug("loading module '" << m_path << "'");

            // if we've already loaded our module, then just return its context
            if (m_handle)
                return &m_ctx;
        }

        // printdebug("update time : " << update_time);
        // printdebug("last update : " << m_last_update);

        // has it changed ? or is this the initialising call
        if (update_time != m_last_update || _init == true)
        {
            // if so, then reload our dll

            if (!_init)
				printdebug("reloading...");

            // unload
            unload();

            // load our module into memory
            load(m_path, update_time);

            // find and run our modules load func
            find_and_load();

            printdebug("module '" << m_name << "' loaded!");

            return &m_ctx;
        }

        return nullptr;
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

        // delete the copied dll file if it exists
        if (!m_copy_path.empty() && std::filesystem::exists(m_copy_path))
        {
            try
            {
                std::filesystem::remove(m_copy_path);
            }
            catch (const std::filesystem::filesystem_error& e)
            {
                printerror(std::format("failed to delete copied dll '{}' : {}", m_name, e.what()));
            }
        }

        printdebug("dll '" << m_name << "' unloaded");

        // only clear the copy path, we dont want to clear m_path bc thats the file
        // we're watching and dont want to lose it
        m_copy_path.clear();

        // clear our context and last update time
        m_ctx           = {};
        m_last_update   = {};
    }

    //
    // tries to find the func with the given name in our dll and returns it
    //
    template<typename type_t>
    dllfn_t<type_t> find(const std::string& _func)
    {
        if (!m_handle)
            printerret(nullptr, "no dll loaded");

        // try to find our function in our dll's memory
        auto fn = RECAST(dllfn_t<type_t>, GetProcAddress(m_handle, _func.c_str()));

        if (!fn)
            printerret(nullptr, std::format("unable to find func '{}' in dll '{}'", _func, m_name));

        return fn;
    }

    //
    // finds the module load function and returns it 
    //
    std::optional<module_load_fn_t> find_load_fn() const
    {
        if (!m_handle)
            printerret(std::nullopt, "dll '" << m_name << "' not loaded");

        // try to find our function in our dll's memory
        auto fn = RECAST(module_load_fn_t, GetProcAddress(m_handle, MOD_LOAD_STR));

        if (!fn)
            printerret(std::nullopt, std::format("unable to find func '{}' in dll '{}', {}", MOD_LOAD_STR, m_name, util::format_win32_error(GetLastError())));

        return fn;
    }

    //
    // runs the given module load function, passing the given module context arg
    //
    void loadfn(const module_load_fn_t& _fn) { (*_fn)(&m_ctx); }

    //
    // finds the load fn then runs it
    //
    void find_and_load()
    {
        // find the module load func
        auto fn = find_load_fn();

        // did we load it ?
        if (fn != std::nullopt)
        {
            // loaded

            printdebug("found load fn for '" << m_name << "'");

            // load our module and get its context
            loadfn(*fn);

            // test to make sure we loaded
            m_ctx.print_info();

            // pass our engine ctx to the module for it to access the subsystems
            m_ctx.on_load(&g_engine);
        }
    }

    //
    // so that we can easily check if this instance has been loaded
    //
    bool operator !() const
    {
        return !m_handle;
    }
};
