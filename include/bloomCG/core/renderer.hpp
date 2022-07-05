#pragma once

#include "bloomCG/buffers/index_buffer.hpp"
#include "bloomCG/buffers/vertex_array.hpp"
#include "bloomCG/core/shader.hpp"

namespace bloom {
  class Renderer {
  private:
  public:
    Renderer();
    ~Renderer();

    void clear() const;
    void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
  };
}  // namespace bloom
