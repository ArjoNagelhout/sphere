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

        // very crude scene inspector that allows viewing all objects
        if (ImGui::Begin("Scene Inspector", &open, 0)) {

            engine::Scene &scene = *engine::engine->scene;

            ImGui::Text("Scene");
            for (int i = 0; i < scene.objects.size(); i++) {
                const auto &obj = scene.objects[i];
//                std::cout << obj->name << std::endl;
                if (ImGui::Button(obj->name.data())) {
                    selectedObjectIndex = i;
                }
            }

            engine::Object &selectedObject = *scene.objects[selectedObjectIndex];
            std::string objString = std::string("Selected object: ") + selectedObject.name;
            ImGui::Text("%s", objString.data());
            ImGui::InputFloat3("Position", glm::value_ptr(selectedObject.localPosition));
            ImGui::InputFloat4("Rotation", glm::value_ptr(selectedObject.localRotation)); // change to represent euler values instead of quaternion
            ImGui::InputFloat3("Scale", glm::value_ptr(selectedObject.localScale));

//            if (ImGui::InputFloat("X", vec)) {
//                std::cout << "input float true" << std::endl;
//            }
            //for (auto const &obj : scene.objects) {

            //}

            ImGui::End();
        }
    }
}