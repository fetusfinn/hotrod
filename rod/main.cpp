//
// hotloading dll
//
#include <print>
#include <thread>

#define HOT_EXPORT extern "C" _declspec(dllexport)

//
// the function we're gonna be loading from our program
//
HOT_EXPORT void dllmain()
{
	std::print("uhhhh\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::print("ummmmmm\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::print("uuhhmmmm\n");

	return;

	std::print("dllz NUTZ\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::print("HAH\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::print("GOTTEEM\n");
}