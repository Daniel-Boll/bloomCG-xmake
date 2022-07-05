#pragma once

#include <bloomCG/core/common.hpp>
#include <bloomCG/core/core.hpp>

namespace bloom {
  namespace scene {
    class Scene {
    public:
      Scene() {}
      virtual ~Scene() {}

      virtual void onUpdate(float deltaTime) {}
      void onSceneRender();
      virtual void onRender(float deltaTime) {}
      virtual void onImGuiRender() {}

      float __deltaTime, __lastFrame;
    };

    class Menu : public Scene {
    private:
      // A list of all scenes that can be loaded
      // Pair of scene name and a scene callback factory
      std::vector<std::pair<std::string, std::function<Scene *()>>> m_scenes;
      Scene *&m_current;

    public:
      Menu(Scene *&current);
      ~Menu() {}

      void onRender(float deltatTime) override { GLCall(glad_glClearColor(.0f, .0f, .0f, 1.0f)); }

      void onImGuiRender() override;
      template <typename T> void registerScene(const std::string &name) {
        fmt::print("Registering scene {}\n", name);

        m_scenes.push_back(std::make_pair(name, []() { return new T(); }));
      }
    };
  }  // namespace scene
}  // namespace bloom
