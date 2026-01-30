# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a Windows C++ DLL hot-reloading system that allows dynamic reloading of a DLL without restarting the host application. The project demonstrates how to implement live code updates during development.

## Project Structure

- **hotrod/** - Host application that loads and hot-reloads the DLL
  - `main.cpp` - Entry point that loads rod.dll and monitors for changes
  - `dll.h` - DLL wrapper class (`dll_t`) that handles loading, unloading, and reloading
  - `print.h` - Logging/printing utilities with macros for formatted output

- **rod/** - The hot-reloadable DLL
  - `main.cpp` - DLL implementation with exported functions (uses `HOT_EXPORT` macro)

## Build System

This is a Visual Studio 2022 C++ solution (`hotrod.sln`) with two projects:

1. **hotrod** (Application) - Console executable
2. **rod** (DynamicLibrary) - Hot-reloadable DLL

### Building

Use MSBuild from Visual Studio Developer Command Prompt or PowerShell:

```powershell
# Build entire solution
msbuild hotrod.sln /p:Configuration=Debug /p:Platform=x64

# Build specific project
msbuild hotrod\hotrod.vcxproj /p:Configuration=Debug /p:Platform=x64
msbuild rod\rod.vcxproj /p:Configuration=Debug /p:Platform=x64

# Clean and rebuild
msbuild hotrod.sln /t:Rebuild /p:Configuration=Debug /p:Platform=x64
```

Alternatively, use Visual Studio IDE (F7 to build, Ctrl+Shift+B for solution).

### Running

```powershell
# Run the host application (from solution root)
x64\Debug\hotrod.exe
```

The application will continuously reload rod.dll when it detects file changes.

## Architecture Details

### Hot-Reloading Mechanism

The host application (`hotrod/main.cpp`) implements a continuous reload loop:

1. Loads `rod.dll` using the `dll_t` wrapper class
2. Retrieves the `dllmain` function pointer via `GetProcAddress`
3. Every 5 seconds: checks if DLL file timestamp changed via `std::filesystem::last_write_time`
4. If changed: unloads the old DLL with `FreeLibrary`, loads the new version

### DLL Interface Contract

Functions exported from the DLL must use the `HOT_EXPORT` macro:
```cpp
#define HOT_EXPORT extern "C" _declspec(dllexport)
```

The host expects a `void dllmain()` function signature. The `dll_t::find_fn<T>()` template method allows loading functions with different return types.

### Key Components

**dll_t class (hotrod/dll.h:23-137)**:
- Wraps Windows `HMODULE` handle
- `load()` - Uses `LoadLibraryA` to load DLL into memory
- `reload()` - Checks file modification time and reloads if changed
- `unload()` - Calls `FreeLibrary` to release DLL
- `find_fn<T>()` - Template method to retrieve exported function pointers with type safety

**print.h macros**:
- `printmsg()` - Standard logging with "[hot]" prefix
- `printdebug()` - Debug messages with "[debug]" prefix
- `printerror()` - Error messages to stderr
- `printerret(msg, ret)` - Error logging + early return pattern

## Language Features

Both projects use **C++23** (`stdcpplatest`):
- `<print>` module for formatted output
- `<format>` for string formatting
- `std::filesystem` for file operations
- `std::chrono_literals` for time literals (e.g., `5s`)

## Development Workflow

1. Build the solution to generate initial `rod.dll`
2. Run `hotrod.exe` - it will load and execute the DLL in a loop
3. Modify code in `rod/main.cpp`
4. Rebuild just the `rod` project - the running application will detect and reload automatically
5. Observe changes without restarting the host process

## Platform Requirements

- **Windows only** (uses Windows API: `LoadLibraryA`, `GetProcAddress`, `FreeLibrary`)
- **Visual Studio 2022** (Platform Toolset v143)
- **C++23 Standard Library** with modules support
- **Windows 10 SDK**
