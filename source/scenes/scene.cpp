#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <bloomCG/scenes/scene.hpp>

namespace bloom {
  namespace scene {
    void Scene::onSceneRender() {
      float currentFrame = glfwGetTime();
      __deltaTime = currentFrame - __lastFrame;
      __lastFrame = currentFrame;

      onUpdate(__deltaTime);
      onRender(__deltaTime);
    }

    // Menu
    Menu::Menu(Scene*& currentScenePointer) : m_current(currentScenePointer) {}

    void Menu::onImGuiRender() {
      for (auto& [name, callback] : m_scenes)
        if (ImGui::Button(name.c_str())) m_current = callback();
    }
  }  // namespace scene
}  // namespace bloom
