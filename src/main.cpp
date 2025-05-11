#include "Game/Scene/SingleModeScene.h"
#include "Game/Scene/StageScene.h"
#include "al/Base/String.h"
#include "al/Controller/ControllerUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "hk/util/hash.h"
#include "nn/socket.h"
#include "pe/BunbunMod.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/EchoEmitterMod.h"
#include "pe/Exception.h"
#include "pe/Execute/ExecuteDirectorExtensions.h"
#include "pe/Factory/ProjectActorFactory.h"
#include "pe/Hacks/FSHacks.h"
#include "pe/Hacks/PlacementHolderMod.h"
#include "pe/Hacks/Tweaks.h"
#include "pe/Util/Log.h"

extern "C" void hkMain() {
    {
        constexpr size_t poolSize = 0xC0000;
        void* pool = malloc(poolSize);
        nn::socket::Initialize(pool, poolSize, 0x4000, 0xe);
    }

    pe::initUserExceptionHandler();
    pe::initProjectActorFactoryHook();
    pe::initBunbunModHooks();
    pe::initEchoEmitterModHooks();
    pe::initPlacementHolderModHooks();
    pe::initExecuteDirectorExtensionHooks();
    pe::installTweaks();
    pe::gui::initDbgGuiHooks();
    //   pe::initWiiUModHooks();
}
