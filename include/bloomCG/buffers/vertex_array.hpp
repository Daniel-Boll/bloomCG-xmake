#pragma once

#include <bloomCG/buffers/vertex_buffer.hpp>
#include <bloomCG/buffers/vertex_buffer_layout.hpp>

namespace bloom {

  class VertexArray {
  private:
    uint32_t m_rendererID;
  public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

    void addBuffer(const VertexBuffer &buffer, const VertexBufferLayout &layout);
  };
}  // namespace bloom
