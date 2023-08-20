#include "engine.h"
#include "editor.h"
#include <glm/gtc/type_ptr.hpp>

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

            if (ImGui::BeginMenu("Create")) {
                ImGui::MenuItem("Cube");

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        engine::Scene &scene = *engine::engine->scene;

        // very crude scene inspector that allows viewing all objects
        if (ImGui::Begin("Scene Inspector")) {
            for (int i = 0; i < scene.objects.size(); i++) {
                const auto &obj = scene.objects[i];
//                std::cout << obj->name << std::endl;
                if (ImGui::Button(obj->name.data())) {
                    selectedObjectIndex = i;
                }
            }

            ImGui::End();
        }

        // should use reflection (or some crude implementation of it to render the right UI element.
        // now it's hardcoded, but it should be

        if (ImGui::Begin("Object inspector")) {
            engine::Object &selectedObject = *scene.objects[selectedObjectIndex];
            ImGui::LabelText("Name", "%s", selectedObject.name.data());
            ImGui::InputFloat3("Position", glm::value_ptr(selectedObject.localPosition));
            ImGui::InputFloat4("Rotation", glm::value_ptr(selectedObject.localRotation)); // change to represent euler values instead of quaternion
            ImGui::InputFloat3("Scale", glm::value_ptr(selectedObject.localScale));

            ImGui::End();
        }

        projectBrowser.render();

        // we need the following systems:
        // asset database
        // a folder that stores the project information in json format, with a list of source assets + their import rules.
    }
}