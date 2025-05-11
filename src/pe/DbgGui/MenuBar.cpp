#include "pe/DbgGui/MenuBar.h"
#include "imgui.h"
#include "pe/Util/PlayerModelNameUtil.h"
#include "pe/Util/Version.h"

namespace pe {
    namespace gui {

        void MenuBar::update() {
        }

        void MenuBar::draw() {
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("Windows")) {
                    ImGui::Checkbox("Demo Window", &getDbgGuiSharedData().showDemoWindow);
                    ImGui::Checkbox("Log", &getDbgGuiSharedData().showLog);
                    ImGui::Checkbox("HeapViewer", &getDbgGuiSharedData().showHeapViewer);
                    ImGui::Checkbox("ActorBrowser", &getDbgGuiSharedData().showActorBrowser);
                    ImGui::Checkbox("Hacks", &getDbgGuiSharedData().showHacks);
                    ImGui::Checkbox("RCSCamera", &getDbgGuiSharedData().showRCSCamera);
                    ImGui::Checkbox("Multiplayer", &getDbgGuiSharedData().showMultiplayer);
                    ImGui::Checkbox("PoseViewer", &getDbgGuiSharedData().showPoseViewer);
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                ImGui::EndMainMenuBar();
            }
        }

    } // namespace gui
} // namespace pe
