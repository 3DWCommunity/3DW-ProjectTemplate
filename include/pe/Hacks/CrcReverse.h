#pragma once

#include <sead/basis/seadTypes.h>

namespace pe {

    void installDebugCrc32HashCollector();
    void createCrc32ReverseHeap();
    const char* reverseCrc32(u32 hash);

    void showCrc32ReverseImGuiWindow();

} // namespace pe
