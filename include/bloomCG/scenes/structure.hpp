#pragma once

#include <bloomCG/scenes/scene.hpp>

namespace bloom {
  namespace scene {
    class Structure : public Scene {
    private:
    public:
      Structure();

      void onUpdate(const float deltaTime) override;
      void onRender(const float deltaTime) override;
      void onImGuiRender() override;
    };
  }  // namespace scene
}  // namespace bloom
