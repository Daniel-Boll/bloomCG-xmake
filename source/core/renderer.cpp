#include <bloomCG/core/core.hpp>
#include <bloomCG/core/renderer.hpp>

namespace bloom {
  ImDrawList* Renderer::s_viewportDrawList = nullptr;
  float Renderer::s_viewportWidth = 0.0f;
  float Renderer::s_viewportHeight = 0.0f;
  float Renderer::s_viewportX = 0.0f;
  float Renderer::s_viewportY = 0.0f;

  void Renderer::clear() const { GLCall(glad_glClear(GL_COLOR_BUFFER_BIT)); }
}  // namespace bloom
