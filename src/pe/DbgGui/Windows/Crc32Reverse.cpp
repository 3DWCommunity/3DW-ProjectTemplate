#include "pe/DbgGui/Windows/Crc32Reverse.h"
#include "Game/MapObj/DisasterModeController.h"
#include "Game/Player/PlayerActionNode.h"
#include "Game/Player/PlayerActor.h"
#include "Game/Util/SceneObjUtil.h"
#include "al/LiveActor/ActorActionFunction.h"
#include "al/Nerve/NerveFunction.h"
#include "imgui.h"
#include "pe/DbgGui/DbgGui.h"
#include "pe/Hacks/CrcReverse.h"
#include "pe/Util/Nerve.h"
#include "pe/Util/PlayerModelNameUtil.h"
#include "pe/Util/Type.h"

namespace pe {
    namespace gui {

        Crc32Reverse::Crc32Reverse() {
        }

        void Crc32Reverse::update() {
        }

        void Crc32Reverse::draw() {
            if (getDbgGuiSharedData().showCrc32Reverse) {
                if (ImGui::Begin("Crc32Reverse", &getDbgGuiSharedData().showCrc32Reverse)) {
                    pe::showCrc32ReverseImGuiWindow();
                }
                ImGui::End();
            }
        }

    } // namespace gui
} // namespace pe
