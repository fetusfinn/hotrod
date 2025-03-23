//
//
//
#include <filesystem>
#include <thread>
#include <chrono>
#include <iostream>

#include "dll.h"    // includes print.h

// so that we can type things like 10s or 50ms
using namespace std::chrono_literals;

//
//
//
int main()
{
    // load our dll
    dll_t dll("rod.dll");

    // find our dllmain func
    auto dllmain = dll.find_fn<void>("dllmain");

    while (true)
    {
        // check if we should reload
        dll.reload();

        // run our imported func
        if (dllmain)
            dllmain();

        std::this_thread::sleep_for(5s);
    }

    dll.unload();

    return 0;
}