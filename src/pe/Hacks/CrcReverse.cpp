#include "pe/Hacks/CrcReverse.h"
#include "container/seadTList.h"
#include "filedevice/seadFileDevice.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "helpers/fsHelper.h"
#include "hk/Result.h"
#include "hk/diag/diag.h"
#include "hk/hook/Trampoline.h"
#include "hk/os/Mutex.h"
#include "hk/ro/RoUtil.h"
#include "hk/util/FixedCapacityArray.h"
#include "imgui.h"
#include "nn/fs.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Util/Log.h"
#include "thread/seadCriticalSection.h"
#include <cstdio>
#include <sead/container/seadBuffer.h>
#include <sead/container/seadListImpl.h>
#include <sead/heap/seadFrameHeap.h>
#include <sead/heap/seadHeapMgr.h>

namespace pe {

    static bool sInstalled = false;
    static sead::Heap* sHeap = nullptr;

    struct HashEntry {
        u32 hashSource = 0;
        u32 hash = 0;
        const char* str = nullptr;
    };

    static hk::util::FixedCapacityArray<HashEntry, 0x8000> sEntries;
    static hk::os::Mutex sEntriesCriticalSection;

    static bool isAlreadyAdded(u32 hash) {
        for (int i = 0; i < sEntries.size(); i++)
            if (sEntries[i].hash == hash)
                return true;
        return false;
    }

    void createCrc32ReverseHeap() {
        if (!sInstalled)
            return;

        sHeap = sead::FrameHeap::create(1024 * 768, "MurmurReverseHeap", pe::gui::getPeepaHeap(), 8, sead::FrameHeap::cHeapDirection_Forward, false);
    }

    static void addHash(u32 hash, const char* str, ptr lr) {
        if (sInstalled && sHeap) {
            sEntriesCriticalSection.lock();
            if (!isAlreadyAdded(hash)) {
                size_t length = strlen(str);
                char* dup = (char*)sHeap->alloc(length + 1);
                memcpy(dup, str, length + 1);

                uintptr_t source = lr - hk::ro::getMainModule()->range().start();
                sEntries.add({ u32(source), hash, dup });

                sEntries.sort([](const HashEntry& a, const HashEntry& b) -> bool {
                    return a.hash < b.hash;
                });
            }
            sEntriesCriticalSection.unlock();
        }
    }

    HkTrampoline<u32, const char*> strHashHook = hk::hook::trampoline([](const char* str) -> u32 {
        uintptr_t lr;
        __asm("mov %[lr], lr" : [lr] "=r"(lr));

        u32 hash = strHashHook.orig(str);

        addHash(hash, str, lr);

        return hash;
    });

    HkTrampoline<u32, const void*, u32> dataHashHook = hk::hook::trampoline([](const void* data, u32 len) -> u32 {
        uintptr_t lr;
        __asm("mov %[lr], lr" : [lr] "=r"(lr));

        u32 hash = dataHashHook.orig(data, len);

        ptr lrModule = lr - hk::ro::getMainModule()->range().start();
        if (lrModule != 0x008a30dc && lrModule != 0x008a31dc)
            addHash(hash, reinterpret_cast<const char*>(data), lr);

        return hash;
    });

    void installDebugCrc32HashCollector() {
        strHashHook.installAtSym<"_ZN4sead9HashCRC3214calcStringHashEPKc">();
        dataHashHook.installAtSym<"_ZN4sead9HashCRC328calcHashEPKvj">();

        sInstalled = true;
    }

    const char* reverseCrc32(u32 hash) {
        if (!sInstalled)
            return nullptr;

        for (int i = 0; i < sEntries.size(); i++)
            if (sEntries[i].hash == hash)
                return sEntries[i].str;

        return nullptr;
    }

    static int sDebugIndex = 0;

    void showCrc32ReverseImGuiWindow() {
        ImGui::InputInt("Index", &sDebugIndex);
        ImGui::PushID("index_slider");
        ImGui::SliderInt("Index", &sDebugIndex, 0, sEntries.size() - 1);
        ImGui::PopID();

        ImGui::Text("Num Entries: %zu", sEntries.size());

        if (sDebugIndex < 0)
            sDebugIndex = 0;
        if (sDebugIndex >= sEntries.size())
            sDebugIndex = sEntries.size() - 1;

        const auto& entry = sEntries[sDebugIndex];

        ImGui::Text("%08x: %s", entry.hash, entry.str ?: "nullptr");
        ImGui::Text("Possible Hash source: %08x", entry.hashSource);

        constexpr static char cFilePath[] = "sd:/hashes.txt";

        if (ImGui::Button("Dump to txt")) {
            auto calcFileSize = []() -> size {
                size_t size = 0;
                for (int i = 0; i < sEntries.size(); i++) {
                    const auto& entry = sEntries[i];
                    char buffer[256];
                    snprintf(buffer, sizeof(buffer), "%08x: %s\n", entry.hash, entry.str ?: "nullptr");
                    size += strlen(buffer);
                }
                return size;
            };
            if (FsHelper::isFileExist(cFilePath)) {
                nn::fs::DeleteFile(cFilePath);
            }
            nn::fs::CreateFile(cFilePath, calcFileSize());

            nn::fs::FileHandle fileHandle;
            hk::Result rc = nn::fs::OpenFile(&fileHandle, cFilePath, nn::fs::OpenMode::OpenMode_Write).GetInnerValueForDebug();
            if (rc.succeeded()) {
                u64 offset = 0;
                for (int i = 0; i < sEntries.size(); i++) {
                    const auto& entry = sEntries[i];
                    char buffer[256];
                    snprintf(buffer, sizeof(buffer), "%08x: %s\n", entry.hash, entry.str ?: "nullptr");
                    u64 size = strlen(buffer);
                    nn::fs::WriteFile(fileHandle, offset, buffer, size, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
                    offset += size;
                }
                nn::fs::CloseFile(fileHandle);
            } else {
                pe::err("Failed to open file: %x", rc.getValue());
            }
        }
    }

} // namespace pe
