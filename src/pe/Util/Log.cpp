#include "pe/Util/Log.h"
#include "heap/seadHeapMgr.h"
#include "hk/diag/diag.h"
#include "hk/os/Thread.h"
#include "hk/svc/api.h"
#include "nn/diag.h"
#include "thread/seadDelegateThread.h"
#include <sead/heap/seadExpHeap.h>
#include <span>

namespace pe {

    constexpr s32 sMaxLines = 8192;

    static sead::ExpHeap* sLogHeap;
    static sead::PtrArray<LogType> sLogLines;

    extern "C" void hkLogSink(const char* msg, size len) {
        hk::svc::OutputDebugString(msg, len);

        volatile auto put = nn::diag::detail::PutImpl;
        if (put != nullptr)
            put({
                    __LINE__,
                    __FILE__,
                    __PRETTY_FUNCTION__,
                },
                msg, len);
    }

    void initializeLog(sead::Heap* parent) {
        sLogHeap = sead::ExpHeap::create(1024 * 256, "LogHeap", parent, 8, sead::ExpHeap::cHeapDirection_Forward, false);
        sLogLines.allocBuffer(sMaxLines, sLogHeap);

        log("Log initialized");
    }

    sead::PtrArray<LogType>& getLogLines() {
        return sLogLines;
    }

    char* addLog(LogType type, size_t len) {
        LogType* data = reinterpret_cast<LogType*>(sLogHeap->alloc(len + 2));
        *data = type;
        while (sLogLines.size() >= sMaxLines or sLogHeap->getFreeSize() < len + 256)
            sLogHeap->free(sLogLines.popFront());
        sLogLines.pushBack(data);
        return reinterpret_cast<char*>(data) + 1;
    }

    bool& shouldLogWindowScrollDown() {
        static bool should = false;
        return should;
    }

#define PE_UTIL_LOG_TEMPLATE(NAME, TYPE)               \
    void NAME(const char* fmt, ...) {                  \
        va_list arg;                                   \
        va_start(arg, fmt);                            \
        size_t size = vsnprintf(nullptr, 0, fmt, arg); \
        char* msg = addLog(LogType::TYPE, size);       \
        vsnprintf(msg, size + 1, fmt, arg);            \
        va_end(arg);                                   \
        shouldLogWindowScrollDown() = true;            \
        hk::diag::logLine(msg);                        \
    }

    PE_UTIL_LOG_TEMPLATE(log, Log);
    PE_UTIL_LOG_TEMPLATE(warn, Warning);
    PE_UTIL_LOG_TEMPLATE(err, Error);

#undef PE_UTIL_LOG_TEMPLATE
#define PE_UTIL_LOG_TEMPLATE(NAME, TYPE)               \
    void NAME(const char* fmt, std::va_list arg) {     \
        size_t size = vsnprintf(nullptr, 0, fmt, arg); \
        char* msg = addLog(LogType::TYPE, size);       \
        vsnprintf(msg, size + 1, fmt, arg);            \
        shouldLogWindowScrollDown() = true;            \
        hk::diag::logLine(msg);                        \
    }

    PE_UTIL_LOG_TEMPLATE(log, Log);
    PE_UTIL_LOG_TEMPLATE(warn, Warning);
    PE_UTIL_LOG_TEMPLATE(err, Error);

} // namespace pe
