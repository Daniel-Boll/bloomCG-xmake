#include <bloomCG/buffers/vertex_buffer.hpp>
#include <bloomCG/core/core.hpp>

namespace bloom {
  VertexBuffer::VertexBuffer(const void* data, uint32_t size) {
    GLCall(glad_glGenBuffers(1, &m_rendererID));
    GLCall(glad_glBindBuffer(GL_ARRAY_BUFFER, m_rendererID));
    GLCall(glad_glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
  }

  VertexBuffer::~VertexBuffer() { GLCall(glad_glDeleteBuffers(1, &m_rendererID)); }

  void VertexBuffer::bind() const { GLCall(glad_glBindBuffer(GL_ARRAY_BUFFER, m_rendererID)); }

  void VertexBuffer::unbind() const { GLCall(glad_glBindBuffer(GL_ARRAY_BUFFER, 0)); }
}  // namespace bloom
