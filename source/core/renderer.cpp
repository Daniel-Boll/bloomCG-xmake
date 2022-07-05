#include <bloomCG/core/renderer.hpp>

#include <bloomCG/core/core.hpp>

namespace bloom {

  Renderer::Renderer() {}

  Renderer::~Renderer() {}

  void Renderer::clear() const { GLCall(glad_glClear(GL_COLOR_BUFFER_BIT)) }

  void Renderer::draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const {
    shader.bind();
    va.bind();
    ib.bind();
    GLCall(glad_glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
  }
}  // namespace bloom
