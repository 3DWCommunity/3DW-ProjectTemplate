#pragma once

#include <cstdarg>
#include <cstdio>
#include <sead/container/seadPtrArray.h>
#include <sead/heap/seadHeap.h>

namespace pe {

    enum class LogType : u8
    {
        Log,
        Warning,
        Error
    };

    void initializeLog(sead::Heap* parent);
    sead::PtrArray<LogType>& getLogLines();
    char* addLog(LogType type, size_t len);
    bool& shouldLogWindowScrollDown();

    void log(const char* fmt, ...);
    void warn(const char* fmt, ...);
    void err(const char* fmt, ...);
    void log(const char* fmt, std::va_list arg);
    void warn(const char* fmt, std::va_list arg);
    void err(const char* fmt, std::va_list arg);

} // namespace pe
