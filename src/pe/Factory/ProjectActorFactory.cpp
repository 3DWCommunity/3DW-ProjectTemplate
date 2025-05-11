#include "pe/Factory/ProjectActorFactory.h"
#include "hk/hook/InstrUtil.h"
#include "hk/ro/RoUtil.h"
#include "pe/Enemy/Foomin.h"
#include "pe/MapObj/CoinVideoPlayer.h"
#include "pe/MapObj/NeedleSwitchParts.h"
#include "pe/MapObj/PatanPanel.h"
#include "pe/MapObj/PatanPanelStarter.h"

constexpr static al::ActorFactoryTableEntry cCustomActorEntries[] = {
    { "Foomin", pe::createActorFunction<pe::Foomin> },
    { "PatanPanel", pe::createActorFunction<pe::PatanPanel> },
    { "PatanPanelStarter", pe::createActorFunction<pe::PatanPanelStarter> },
    { "NeedleSwitchParts", pe::createActorFunction<pe::NeedleSwitchParts> },
};

pe::ProjectActorFactory::ProjectActorFactory() {
    static al::ActorFactoryTableEntry
        outEntries[sizeof(cCustomActorEntries) / sizeof(al::ActorFactoryTableEntry) + sizeof(::ProjectActorFactory::sActorEntries) / sizeof(al::ActorFactoryTableEntry)];
    static bool isInitialized = false;

    if (!isInitialized) {
        int i = 0;

        for (const al::ActorFactoryTableEntry& entry : cCustomActorEntries) {
            outEntries[i] = entry;
            i++;
        }

        for (const al::ActorFactoryTableEntry& entry : ::ProjectActorFactory::sActorEntries) {
            outEntries[i] = entry;
            i++;
        }
        isInitialized = true;
    }

    initFactory(outEntries);
}

static void projectActorFactoryHook(ProjectActorFactory* factory) { new (factory) pe::ProjectActorFactory(); }

void pe::initProjectActorFactoryHook() {
    hk::hook::writeBranch(hk::ro::getMainModule(), 0x003d86b0, projectActorFactoryHook);
}
