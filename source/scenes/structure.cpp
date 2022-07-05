#include <imgui.h>

#include <bloomCG/core/core.hpp>
#include <bloomCG/scenes/structure.hpp>

namespace bloom {
  namespace scene {
    Structure::Structure() {
      GLCall(glad_glEnable(GL_BLEND));
      GLCall(glad_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    }

    void Structure::onUpdate(const float deltaTime) {}

    void Structure::onRender(const float deltaTime) { GLCall(glad_glClear(GL_COLOR_BUFFER_BIT)); }

    void Structure::onImGuiRender() {}

  }  // namespace scene
}  // namespace bloom
