# Project TODO List

This document consolidates all TODOs found throughout the codebase, prioritizes them, and includes additional recommendations for improvement.

---

## Part 1: Existing TODOs (from codebase comments)

### Priority 1 - Critical/Core Functionality

#### 1. Implement proper module unloading
- **Location**: `rod/dllmain.cpp:97` in `MOD_UNLOAD_FN()`
- **TODO**: "unload everything"
- **Why Priority 1**: Proper cleanup is essential for preventing memory leaks and resource exhaustion, especially in a hot-reloading system where modules are repeatedly loaded/unloaded.
- **Impact**: High - Memory leaks and resource issues

#### 2. Replace generic module context with typed contexts
- **Locations**:
  - `hotrod/dll.h:27` - "replace module_context_t* with void* so that we can use different context structs"
  - `shared/context.h:70-71` - "need to create custom contexts for each module" + "maybe have a base context"
  - `shared/context.h:77` - "this is a generic context, need to create custom contexts"
- **Why Priority 1**: This is a fundamental architectural improvement that enables type-safe, module-specific contexts. The current generic context limits flexibility and type safety.
- **Impact**: High - Core architecture, affects all modules
- **Note**: There's already a `mod_test_ctx_t` at context.h:182-188 that needs testing

#### 3. Check if DLL has already been loaded (verification needed)
- **Location**: `hotrod/dll.h:32` in `dll_t` struct comment
- **TODO**: "check if dll has already been loaded"
- **Why Priority 1**: Prevents duplicate loading and potential conflicts. However, `load_or_get()` (dll.h:86-114) appears to already implement this via the m_pool lookup.
- **Impact**: Medium - May already be complete, needs verification
- **Action**: Review if this TODO is already satisfied by existing implementation

### Priority 2 - Important Enhancements

#### 4. Add arguments to module lifecycle functions
- **Location**: `shared/context.h:100` in `module_context_t`
- **TODO**: "add arguments"
- **Why Priority 2**: Currently lifecycle functions have limited parameters. Adding arguments (like delta time, input state, etc.) would make modules more functional.
- **Impact**: Medium-High - Improves module capabilities
- **Examples**:
  - `CTX_UPDATE_FN` could take delta time
  - `CTX_INPUT_FN` could take input state
  - See example at context.h:186-188 (`mod_test_ctx_t`)

#### 5. Automatic DLL discovery
- **Location**: `hotrod/main.cpp:60` in `main()`
- **TODO**: "automatically find all dlls in a certain folder"
- **Why Priority 2**: Would eliminate manual DLL loading. Currently hardcoded to "rod.dll".
- **Impact**: Medium - Quality of life improvement, enables plugin architecture
- **Implementation**: Use `std::filesystem::directory_iterator` to scan a "plugins" or "modules" folder

#### 6. Implement subsystem name management
- **Location**: `shared/subsystem.h:12`
- **TODO**: "should probably have macros for all the builtin subsystem names, or maybe an enum"
- **Why Priority 2**: Prevents typos and provides compile-time checking for subsystem names. Currently using string literals like "test".
- **Impact**: Medium - Code quality, reduces bugs
- **Suggestion**: Use `constexpr const char*` or enum class

#### 7. Add more lifecycle hooks
- **Location**: `shared/context.h:109` in `module_context_t`
- **TODO**: "add more functions as we create more hooks for functions"
- **Why Priority 2**: Current hooks are basic. More hooks enable richer module behavior.
- **Impact**: Medium - Extensibility
- **Suggestions**:
  - `on_render()` for graphics
  - `on_fixed_update()` for physics
  - `on_config_changed()` for settings
  - `on_pre_reload()` / `on_post_reload()` for state preservation

### Priority 3 - Code Quality & Refactoring

#### 8. Move DLL pool to manager class
- **Location**: `hotrod/dll.h:37` - `dll_t::m_pool`
- **TODO**: "maybe move into some sort of manager ?"
- **Why Priority 3**: Static member variable is functional but not ideal architecture. A manager class would provide better control and lifecycle management.
- **Impact**: Low-Medium - Code organization
- **Benefits**: Centralized management, easier to add features like hot-reload all, dependency resolution

#### 9. Remove or clean up unused subsystem getter
- **Location**: `shared/subsystem.h:66` in `subsystem_manager_t::get_raw()`
- **TODO**: "we probably dont need this"
- **Why Priority 3**: Dead code removal keeps codebase clean. The templated `find<T>()` already provides type-safe access.
- **Impact**: Low - Code cleanliness
- **Action**: Verify it's unused, then remove or mark as deprecated

#### 10. Rename subsystem_info_t
- **Location**: `shared/context.h:31` in `subsystem_info_t`
- **TODO**: "maybe rename this"
- **Why Priority 3**: Name could be clearer about its purpose (it's a wrapper/descriptor).
- **Impact**: Low - Code readability
- **Suggestions**: `subsystem_descriptor_t`, `subsystem_entry_t`, `registered_subsystem_t`

#### 11. Relocate global engine context
- **Location**: `shared/context.h:62` - `extern engine_context_t g_engine`
- **TODO**: "move elsewhere, just here for now"
- **Why Priority 3**: Global variables in headers are generally not ideal, though the `#ifndef HOT_MOD` guard helps.
- **Impact**: Low - Code organization
- **Suggestion**: Move to engine-specific header or use dependency injection

#### 12. Consider subsystem naming convention
- **Location**: `shared/context.h:13`
- **TODO**: "maybe prefix subsystem contexts using sub_ or subctx_"
- **Why Priority 3**: Already using `sub_test_ctx_t` which follows this pattern, so it's mostly done.
- **Impact**: Low - Consistency
- **Action**: Ensure all future subsystem contexts follow the `sub_*_ctx_t` pattern

#### 13. Test custom module context
- **Location**: `shared/context.h:182` - `mod_test_ctx_t`
- **TODO**: "need to test"
- **Why Priority 3**: There's an example custom module context struct but it's untested.
- **Impact**: Low - Validation
- **Action**: Create a test module that uses `mod_test_ctx_t` to verify the pattern works

#### 14. Replace macros for production
- **Location**: `shared/macros.h:83`
- **TODO**: "should probably not use macros for the release version"
- **Why Priority 3**: Macros reduce debuggability and can cause issues. Consider constexpr or inline functions where possible.
- **Impact**: Low - Code quality
- **Note**: Some macros (like `MOD_LOAD_FN`) may need to stay for string conversion

#### 15. Consider renaming MOD_LOAD_FN
- **Location**: `shared/macros.h:86`
- **TODO**: "maybe rename to 'get'"
- **Why Priority 3**: Minor naming consideration. Current name is clear.
- **Impact**: Very Low - Personal preference
- **Suggestion**: Keep current name unless there's confusion

#### 16. Remove test-only fields from module context
- **Location**: `shared/context.h:88` in `module_context_t`
- **TODO**: "dont really need the desc and author, just for testing"
- **Why Priority 3**: These fields are useful for metadata display and plugin management. Consider keeping them.
- **Impact**: Very Low - Code cleanliness
- **Suggestion**: Keep them; they're standard for plugin systems and have minimal cost

---

## Part 2: Recommended Improvements (Not in codebase TODOs)

### Architecture & Design

#### 17. Implement proper error/result types
- **Why**: Replace `printerret` macro pattern with proper error handling using `std::expected` (C++23) or custom Result<T, E> type
- **Priority**: High
- **Benefits**:
  - Better error propagation
  - Compile-time error handling verification
  - Eliminates early-return macros
- **Example**: `std::expected<dll_t*, std::string> load_or_get(...)`

#### 18. Add configuration system
- **Why**: Currently hardcoded values (5s reload interval, "rod.dll" path, "current" folder)
- **Priority**: Medium
- **Suggestions**:
  - JSON/TOML config file
  - Command-line arguments
  - Environment variables
- **Configuration items**:
  - Module search paths
  - Reload interval
  - Debug/release mode
  - Module load order

#### 19. Implement proper logging system
- **Why**: Print macros are basic; need levels, filtering, file output
- **Priority**: Medium
- **Features**:
  - Log levels (trace, debug, info, warn, error, fatal)
  - Log to file with rotation
  - Per-module log filtering
  - Timestamps
  - Thread safety
- **Libraries to consider**: spdlog, or custom lightweight solution

#### 20. Add module dependency system
- **Why**: Modules may depend on other modules being loaded first
- **Priority**: Medium
- **Implementation**:
  - Modules declare dependencies in metadata
  - Dependency graph resolution
  - Load order enforcement
  - Cycle detection
- **Example**: A "graphics" module depends on "window" module

#### 21. State preservation across reloads
- **Why**: Hot-reloading loses all module state, which is problematic for development
- **Priority**: Medium-High
- **Approaches**:
  - Serialize state before unload, deserialize after reload
  - Persist state in engine-managed memory
  - Module provides save/restore callbacks
- **Benefits**: True hot-reload experience for developers

#### 22. Thread safety improvements
- **Why**: No mutex protection on `dll_t::m_pool` or reload operations
- **Priority**: High (if using multithreading)
- **Concerns**:
  - Concurrent access to m_pool
  - Reloading while module is executing
  - Callback function pointers during reload
- **Solution**: Add mutexes, consider RW locks, atomic smart pointers

### Code Quality & Maintainability

#### 23. Add unit tests
- **Why**: No test infrastructure currently
- **Priority**: High
- **Test areas**:
  - DLL loading/unloading
  - Subsystem manager
  - Module lifecycle
  - Error handling paths
  - File watching logic
- **Framework**: Consider Catch2, Google Test, or doctest

#### 24. Add API documentation
- **Why**: Code is well-structured but lacks comprehensive docs
- **Priority**: Medium
- **Suggestions**:
  - Doxygen comments for public API
  - Architecture documentation (complement CLAUDE.md)
  - Module development guide
  - API reference for subsystem creation
  - Example module tutorial

#### 25. Improve error messages
- **Why**: Some errors lack context for debugging
- **Priority**: Low-Medium
- **Examples**:
  - dll.h:195 - "failed to load dll" (which dll? what error code?)
  - dll.h:303 - Include HMODULE value in error
  - Add file paths and line numbers where applicable
- **Already good**: dll.h:326 uses `util::format_win32_error()`

#### 26. Memory management clarification
- **Why**: `dll_t` instances created with `new` in `load_or_get()`, deleted in `remove_from_pool()`
- **Priority**: Medium
- **Concerns**:
  - Manual memory management is error-prone
  - Destructor called by delete, but also manually by pool removal
  - Pool owns raw pointers
- **Suggestions**:
  - Use `std::unique_ptr` in pool
  - Use `std::shared_ptr` if multiple owners needed
  - Document ownership clearly

#### 27. Add CMake build support
- **Why**: Currently Visual Studio-only; CMake enables cross-platform development
- **Priority**: Low (since Windows-specific API used)
- **Benefits**:
  - VS Code support
  - CLion support
  - CI/CD integration
  - Easier to integrate with other build systems
- **Note**: Would need to keep .vcxproj for VS users

### Features & Extensions

#### 28. Module version checking
- **Why**: Context has major/minor version but no validation
- **Priority**: Low-Medium
- **Features**:
  - Engine declares required module version
  - Refuse to load incompatible versions
  - Semantic versioning support
  - ABI compatibility checking

#### 29. Module hot-reload triggers
- **Why**: Currently only time-based (5s polling); need more options
- **Priority**: Low-Medium
- **Alternatives**:
  - File system watcher (ReadDirectoryChangesW on Windows)
  - Manual trigger via input (press R to reload)
  - IPC signal (external tool triggers reload)
  - Build system integration (reload after successful compile)

#### 30. Performance monitoring
- **Why**: No instrumentation for profiling module performance
- **Priority**: Low
- **Metrics**:
  - Module load/reload times
  - Per-module update times
  - Memory usage per module
  - Frame time tracking
- **Output**: Log file, real-time display, or profiler integration

#### 31. Module sandboxing/security
- **Why**: Modules have full access to engine internals
- **Priority**: Low (for prototyping), High (for production/plugin system)
- **Concerns**:
  - Untrusted module code
  - Resource limits (CPU, memory)
  - Filesystem access control
  - API surface limitation
- **Approaches**:
  - Capability-based security
  - API allowlisting
  - Separate process (heavy but secure)

#### 32. Debug visualization
- **Why**: No visual feedback for hot-reload status
- **Priority**: Low
- **Features**:
  - On-screen display showing loaded modules
  - Reload status indicators
  - Error notifications
  - Performance overlay
- **Implementation**: ImGui integration or console dashboard

#### 33. Module reload history/undo
- **Why**: Development workflow enhancement
- **Priority**: Very Low
- **Features**:
  - Keep N previous DLL versions
  - Ability to rollback to previous version
  - Diff visualization
  - Useful for "that last change broke everything" scenarios

---

## Summary Statistics

- **Total Existing TODOs**: 16
- **Total Recommended Improvements**: 17
- **Priority 1 (Critical)**: 3 items
- **Priority 2 (Important)**: 5 items
- **Priority 3 (Quality/Refactoring)**: 8 items
- **Recommended High Priority**: 4 items
- **Recommended Medium Priority**: 9 items
- **Recommended Low Priority**: 4 items

---

## Suggested Implementation Order

1. **Verify existing functionality**: Check if dll.h:32 TODO is already complete
2. **Module cleanup**: Implement proper unloading (rod/dllmain.cpp:97)
3. **Error handling**: Implement Result<> types for better error propagation
4. **Thread safety**: Add mutexes if needed
5. **Testing infrastructure**: Add unit tests for critical paths
6. **Custom contexts**: Complete the module context refactoring
7. **Configuration system**: Remove hardcoded values
8. **Logging system**: Replace print macros
9. **Auto-discovery**: Implement automatic DLL loading
10. **State preservation**: Add hot-reload state management
11. **Documentation**: Add API docs and guides
12. **Everything else**: Tackle remaining items based on need

---

*Generated: 2026-01-06*
*Based on codebase analysis of hotrod hot-reloading system*
