//
//	dll_manager.h | Finn Le Var
//
#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

#include "dll.h"
#include "shared/macros.h"
#include "shared/assert.h"


//
// dll manager class
// handles finding, loading, storing, and managing all dlls
//
class dll_manager_t
{
private:

	// pool of loaded dlls, keyed by filename without extension
	// todo : mutex ?
	std::unordered_map<std::string, dll_t*> m_pool;

	// list of paths we're watching for dlls
	std::vector<std::string> m_paths;

	// whether the manager has been initialised
	bool m_init = false;

private:

	// hide constructor so we can't create more instances
	dll_manager_t() = default;

public:

	//
	// destructor - unloads all dlls
	//
	~dll_manager_t()
	{
		unload_all();
	}

	//
	// initialises the manager with the given paths
	//
	void init(const std::vector<std::string>& _paths = { "." })
	{
		if (m_init)
			printerret(;, "dll manager already initialised");

		m_paths = _paths;
		m_init  = true;

		printdebug("dll manager initialised with " << m_paths.size() << " watch path(s)");

		for (const auto& path : m_paths)
			printdebug("+    " << path);
	}

	//
	// checks if a dll with the given name is loaded
	//
	bool has(const std::string& _name) const
	{
		return m_pool.contains(_name);
	}

	//
	// gets a dll from the pool by name (without extension)
	//
	dll_t* get(const std::string& _name) const
	{
		auto    it = m_pool.find(_name);
		return (it != m_pool.end()) ? it->second : nullptr;
	}

	//
	// loads a dll from the given path, stores it in the pool
	// returns the dll if successful, nullptr if failed
	//
	dll_t* load(const std::filesystem::path& _path)
	{
		// get filename without extension
		const std::string filename = _path.stem().string();

		// check if already loaded
		if (has(filename))
		{
			printdebug("dll '" << filename << "' already loaded, returning existing instance");
			return get(filename);
		}

		printdebug("loading dll '" << filename << "' from '" << _path.string() << "'");

		// create new dll instance
		auto dll = new dll_t(_path.string());

		// check that we loaded successfully
		if (!dll->loaded())
		{
			printerror("failed to load dll '" << filename << "'");
			delete dll;
			return nullptr;
		}

		// store in pool using filename as key
		m_pool[filename] = dll;

		printmsg("dll '" << filename << "' loaded successfully");

		return dll;
	}

	//
	// unloads and removes a dll from the pool
	//
	void unload(const std::string& _name)
	{
		auto it = m_pool.find(_name);

		if (it == m_pool.end())
			printerret(;, "dll '" << _name << "' not found in pool");

		// delete the dll (calls destructor which unloads it)
		delete it->second;

		// remove from pool
		m_pool.erase(it);

		printdebug("dll '" << _name << "' unloaded and removed from pool");
	}

	//
	// unloads all dlls in the pool
	//
	void unload_all()
	{
		printdebug("unloading all dlls...");

		for (auto& [name, dll] : m_pool)
			delete dll;

		m_pool.clear();

		printdebug("all dlls unloaded");
	}

	//
	// discovers all dlls in the watch paths
	// returns the number of dlls found and loaded
	//
	size_t find_and_load()
	{
		if (!m_init)
			printerret(0, "dll manager not initialised");

		size_t loaded_count = 0;

		printdebug("searching for dlls in stored paths...");

		// iterate through each watch path
		for (const auto& watch_path : m_paths)
		{
			// check if the path exists
			if (!std::filesystem::exists(watch_path))
			{
				printerror("path '" << watch_path << "' does not exist");
				continue;
			}

			// check if it's a directory
			if (!std::filesystem::is_directory(watch_path))
			{
				printerror("path '" << watch_path << "' is not a directory");
				continue;
			}

			// iterate through directory
			for (const auto& entry : std::filesystem::directory_iterator(watch_path))
			{
				// skip if not a regular file
				if (!entry.is_regular_file())
					continue;

				// check if it's a dll
				// todo : custom extension
				if (entry.path().extension() != ".dll")
					continue;

				// get the filename without extension
				std::string filename = entry.path().stem().string();

				// skip if it's in the "current" folder (those are our copies)
				if (watch_path.find(CUR_FOLDER) != std::string::npos)
					continue;

				// try to load it
				if (load(entry.path()))
					loaded_count++;
			}
		}

		printmsg("discovery complete : " << loaded_count << " dll(s) loaded");

		return loaded_count;
	}

	//
	// discovers dlls matching a specific name pattern
	// useful for loading specific dlls like "mod_*.dll"
	//
	size_t find_and_load_pattern(const std::string& _pattern)
	{
		if (!m_init)
			printerret(0, "dll manager not initialised");

		size_t loaded_count = 0;

		printdebug("finding dlls matching pattern '" << _pattern << "'...");

		for (const auto& watch_path : m_paths)
		{
			if (!std::filesystem::exists(watch_path) || !std::filesystem::is_directory(watch_path))
				continue;

			for (const auto& entry : std::filesystem::directory_iterator(watch_path))
			{
				if (!entry.is_regular_file() || entry.path().extension() != ".dll")
					continue;

				std::string filename = entry.path().stem().string();

				// skip current folder copies
				if (watch_path.find(CUR_FOLDER) != std::string::npos)
					continue;

				// simple pattern matching (startsWith for now)
				// you could use regex here for more complex patterns
				if (filename.find(_pattern) != std::string::npos)
				{
					if (load(entry.path()))
						loaded_count++;
				}
			}
		}

		printmsg("pattern discovery complete : " << loaded_count << " dll(s) loaded");

		return loaded_count;
	}

	//
	// reloads all dlls in the pool that have been modified
	// returns the number of dlls that were reloaded
	//
	size_t reload_modified()
	{
		size_t reload_count = 0;

		for (auto& [name, dll] : m_pool)
		{
			if (dll->reload())
			{
				// dll was reloaded, run reload callback if present
				dll->m_ctx.on_reload();
				reload_count++;
			}
		}

		return reload_count;
	}

	//
	// updates all loaded dlls (calls their on_update callback)
	//
	void update_all()
	{
		for (auto& [name, dll] : m_pool)
		{
			if (dll->loaded())
			{
				// todo : ? abillity to pass args, probs dont need, just pass via the custom context for each mod
				dll->m_ctx.on_update();
			}
		}
	}

	//
	// gets the number of dlls in the pool
	//
	size_t count() const
	{
		return m_pool.size();
	}

	//
	// dumps info about all loaded dlls
	//
	void dump() const
	{
		printdebug("dumping dll manager state...");
		printdebug("loaded dlls : " << m_pool.size());

		for (const auto& [name, dll] : m_pool)
		{
			printdebug("+    " << name << " @ " << dll->m_path);

			if (dll->loaded())
			{
				printdebug("      status: loaded");
				printdebug("      module : " << dll->m_ctx.name);
			}
			else
			{
				printdebug("      status : not loaded");
			}
		}
	}

	//
	// gets all dll names in the pool
	//
	std::vector<std::string> get_all_names() const
	{
		std::vector<std::string> names;
		names.reserve(m_pool.size());

		for (const auto& [name, dll] : m_pool)
		{
			names.push_back(name);
		}

		return names;
	}

	//
	// gets all dll pointers in the pool
	//
	std::vector<dll_t*> get_all_dlls() const
	{
		std::vector<dll_t*> dlls;
		dlls.reserve(m_pool.size());

		for (const auto& [name, dll] : m_pool)
		{
			dlls.push_back(dll);
		}

		return dlls;
	}

	// make this class a singleton
	MAKE_SINGLETON(dll_manager_t);
};

// create our alias var for easy access
MAKE_SINGLETON_ALIAS(dll_manager_t, dll)
