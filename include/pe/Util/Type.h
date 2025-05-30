#pragma once

#include "Game/Player/PlayerActor.h"
#include "al/LiveActor/LiveActor.h"
#include "hk/ro/RoUtil.h"
#include <type_traits>

namespace pe {
namespace util {

    template <typename T>
    uintptr_t getVft(const T* instance)
    {
        uintptr_t vft = *reinterpret_cast<const uintptr_t*>(instance);
        return vft;
    }

    template <typename T>
    ptrdiff_t getVftOffsetMain(const T* instance)
    {
        return getVft(instance) - hk::ro::getMainModule()->range().start();
    }

    template <typename T, typename F>
    bool checkTypeByVtable(const F* base);

    template <typename T, typename F>
    T* typeCast(F* base)
    {
        if (checkTypeByVtable<T>(base))
            return static_cast<T*>(base);

        return nullptr;
    }

    PlayerActor* checkPlayer(al::LiveActor* instance);

} // namespace util
} // namespace pe
