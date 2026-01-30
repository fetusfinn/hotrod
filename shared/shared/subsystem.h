//
//	subsystem.h | Finn Le Var
//
#pragma once

#include "shared/context.h"		// includes shared/macros.h and shared/print.h

#include <unordered_map>
#include <utility>

//
// all of our different types of subsystems
//
enum subsystem_type_t : int
{
	SUB_UNKNOWN = 0,
	SUB_TEST,

	// todo, just for testing
	SUB_THREAD_POOL,
	SUB_DISPATCHER,
};

//
// returns the string value for the given subsystem type
//
inline const char* to_string(subsystem_type_t _type)
{
	switch (_type)
	{
	case SUB_TEST:			return "SUB_TEST";
	case SUB_THREAD_POOL:	return "SUB_THREAD_POOL";
	case SUB_DISPATCHER:	return "SUB_DISPATCHER";
	default:				return "unknown";
	}
}

//
// subsystem manager for our modules
//
class subsystem_manager_t
{
private:

	// our engine context
	engine_context_t* m_engine = nullptr;

	// list of all of our subsystems as raw void pointers
	std::unordered_map<std::string, void*> m_subsystems;

	// whether our manager has been initialised
	bool m_init = false;

private:

	// hide our constructor so we cant create more
	subsystem_manager_t() = default;

public:

	//
	// initialises our subsystem manager, basically a constructor
	//
	void init(engine_context_t* _engine)
	{
		if (m_init)
			printerret(; , "subsystem manager already initialised");

		m_engine = _engine;

		printdebug("parsing subsystems");

		// iterate over them all
		for (int i = 0; std::cmp_less(i, m_engine->subsystem_count); ++i)
		{
			subsystem_info_t& subsystem = m_engine->subsystems[i];

			printdebug("+    " << subsystem.name);

			// store our subsystem
			m_subsystems.emplace(subsystem.name, subsystem.data);
		}

		m_init = true;

		printdebug("subsystem manager initialised");
	}

	//
	//
	//
	void cleanup()
	{
		m_subsystems.clear();

		m_engine = nullptr;
	}

	//
	// gets the given subsystem as a raw void pointer
	//
	void* get_raw(subsystem_type_t _type) const
	{
		if (m_subsystems.empty())
			printerret(nullptr, "subsystem cache is empty");

		if (_type == SUB_UNKNOWN)
			printerret(nullptr, "unknown subsystem name given");

		const auto& it = m_subsystems.find(to_string(_type));

		if (it == m_subsystems.end())
			printerret(nullptr, "subsystem '" << to_string(_type) << "' does not exist");

		return it->second;
	}

	//
	// finds and returns the subsystem with the given name from our list of cached subsystems
	//
	template<typename subsys_t>
	subsys_t* find(subsystem_type_t _type)
	{
		return CASTTO(subsys_t*, get_raw(_type));
	}


	//
	// prints the names of all of our subsystems
	//
	void dump()
	{
		printdebug("dumping subsystems...");

		for (const auto& [ name, sub ] : m_subsystems)
		{
			printdebug("+    " << name);
		}
	}


	
	// make this class a singleton
	MAKE_SINGLETON(subsystem_manager_t);

};

// create our alias var
MAKE_SINGLETON_ALIAS(subsystem_manager_t, subsystem)