#include "project_browser.h"

#include <imgui.h>

namespace editor {
    ProjectBrowser::ProjectBrowser() = default;

    ProjectBrowser::~ProjectBrowser() = default;

    void ProjectBrowser::render() {
        if (ImGui::Begin("Project Browser")) {

            // list all files in folder
            if (ImGui::Button("Open Folder")) {

            }

            ImGui::End();
        }
    }
}