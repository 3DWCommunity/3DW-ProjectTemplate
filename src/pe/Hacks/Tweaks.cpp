#include "pe/Hacks/Tweaks.h"
#include "al/Base/String.h"
#include "al/LiveActor/LiveActor.h"
#include "al/Placement/PlacementInfo.h"
#include "al/Resource/Resource.h"
#include "al/Scene/LiveActorGroup.h"
#include "heap/seadFrameHeap.h"
#include "hk/diag/diag.h"
#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/hook/a64/Assembler.h"
#include "hk/ro/RoUtil.h"
#include "pe/Util/Log.h"
#include <cstring>

namespace pe {

    // this prevents crashes when using null strings in AnimPlayerSimple (no clue why this ever happens, tho)
    static int animInfoTableStrcmpCheck(const char* a, const char* b) {
        if (a == nullptr or b == nullptr) {
            return 1;
        }
        return strcmp(a, b);
    }

    static void liveActorGroupRegisterActorHook(al::LiveActorGroup* thisPtr, al::LiveActor* actor) {
        HK_ABORT_UNLESS(thisPtr->mSize < thisPtr->mCapacity, "LiveActorGroup with name %s is full (%d)", thisPtr->mName, thisPtr->mCapacity);

        thisPtr->mActors[thisPtr->mSize] = actor;
        thisPtr->mSize++;
    }

    struct AllocFailedCallbackArg {
        sead::Heap* heap;
        size_t allocSize;
        u32 allocAlignment;
    };

    static void allocFailedReporter(void* thisPtr, const AllocFailedCallbackArg* arg) {
        HK_ABORT("Allocation of %zu bytes with alignment %u failed on heap %s with %d bytes free", arg->allocSize, arg->allocAlignment, arg->heap->getName().cstr(), arg->heap->getFreeSize());
    }

    constexpr size cSceneObjHolderSize = 62;

    static sead::FrameHeap* createSceneResourceHeapHook(size_t size, const sead::SafeString& name, sead::Heap* parent, s32 alignment, sead::Heap::HeapDirection direction, bool something) {
        if (al::isEqualString(name, "SceneHeapResource") && size == 136314880 /* 130.0 MB */)
            size = 330 * 1024 * 1024; // need extra memory for all the puppets
        return sead::FrameHeap::tryCreate(size, name, parent, alignment, direction, something);
    }

    HkTrampoline<al::Resource*, al::ResourceSystem*, const sead::SafeString&, const char*> findOrCreateResourceWarn = hk::hook::trampoline([](al::ResourceSystem* thisPtr, const sead::SafeString& archive, const char* sth) -> al::Resource* {
        al::Resource* resource = findOrCreateResourceWarn.orig(thisPtr, archive, sth);
        // uncomment this and bowser furry will crash
        // if (resource == nullptr)
        //    pe::warn("al::ResourceSystem::findOrCreateResource(%p, \"%s\", %s) returned nullptr", thisPtr, archive.cstr(), sth ? sth : "nullptr");

        return resource;
    });

    HkTrampoline<void, void*, void*> initializeGfxMemoryPoolHook = hk::hook::trampoline([](void* memBlock, void* x1) {
        if (memBlock == nullptr)
            pe::err("GPU mem allocation failed");
        initializeGfxMemoryPoolHook.orig(memBlock, x1);
    });

    void installTweaks() {
        hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x008d59ec, animInfoTableStrcmpCheck);
        hk::hook::writeBranch(hk::ro::getMainModule(), 0x008316c0, liveActorGroupRegisterActorHook);
        hk::hook::writeBranch(hk::ro::getMainModule(), 0x00872c30, allocFailedReporter);
        hk::hook::writeBranch(hk::ro::getMainModule(), 0x00720554, createSceneResourceHeapHook);
        hk::hook::a64::assemble<"mov w2, {}">()
            .arg(cSceneObjHolderSize)
            .installAtMainOffset(0x003e624c);
        findOrCreateResourceWarn.installAtSym<"_ZN2al14ResourceSystem20findOrCreateResourceERKN4sead14SafeStringBaseIcEEPKc">();
        initializeGfxMemoryPoolHook.installAtOffset(hk::ro::getMainModule(), 0x00758710);
    }

    void installTweaksAfterInit() {
        // tryGetInitFileIterAndNameHook.installAtSym<"_ZN2al25tryGetInitFileIterAndNameEPNS_9ByamlIterEPN4sead22BufferedSafeStringBaseIcEEPKNS_8ResourceEPKcSA_SA_S8_">();
    }

} // namespace pe
