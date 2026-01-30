//
//	assert.h | Finn Le Var
//
#pragma once

#include <iostream>
#include <format>
#include <source_location>
#include <stdexcept>

//
// std funcs and classes
//
namespace stl
{
	class assert_error : public std::runtime_error
	{
	public:
		assert_error(const std::string& _msg) : std::runtime_error(_msg) { }
	};
}

//
// checks if the given condition is true, then prints a message and throws an exception if it is
//
// we want to pass the condition that we dont want happening, if it passes, then it will error
//
#define ASSERT(_cond, ...) \
do { \
	if ((_cond)) {		\
		const auto  loc = std::source_location::current(); \
		std::string msg = std::format("[assert] {} in {}:{} | {}", loc.function_name(), loc.file_name(), loc.line(), #_cond); \
		if constexpr (sizeof(__VA_ARGS__) > 1) { \
			msg += std::format(" | {}", __VA_ARGS__); \
		} \
		msg += "\n";  \
		std::cerr << msg; \
		throw stl::assert_error(msg); \
	} \
} while(0)
