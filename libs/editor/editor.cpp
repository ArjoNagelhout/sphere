#include "editor.h"

#include <iostream>
#include <imgui.h>

namespace editor {
    Editor::Editor() {
        std::cout << "created editor" << std::endl;
    }

    Editor::~Editor() = default;

    void Editor::render() {
        bool open = true;
        ImGui::ShowDemoWindow(&open);

        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("File")) {

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
        ImGui::Begin("Sphere", &open, 0);
        ImGui::End();
    }
}