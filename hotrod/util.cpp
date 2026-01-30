//
//	util.cpp | Finn Le Var
//
#include "util.h"

//
//
//
namespace util
{
    //
    // gets the error message for the given error code and formats said message
    //
    std::string format_win32_error(DWORD _error_code)
    {
        // buffer to write our message to
        LPSTR buffer = nullptr;

        // get our message
        DWORD size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, _error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&buffer), 0, nullptr);

        std::string message;

        // if there is a message
        if (size && buffer)
        {
            // convert our message to an std::string
            message.assign(buffer, size);

            // remove trailing newlines as FormatMessage often appends '\r' and '\n'
            while (!message.empty() && (message.back() == '\r' || message.back() == '\n'))
                message.pop_back();

            // clear the buffer
            LocalFree(buffer);
        }
        else
        {
            // didnt manage to find the error message, must be an unknown error
            message = "unknown windows error code : " + std::to_string(_error_code);
        }

        return message;
    }
}