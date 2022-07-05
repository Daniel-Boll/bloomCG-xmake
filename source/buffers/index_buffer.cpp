#include <bloomCG/buffers/index_buffer.hpp>
#include <bloomCG/core/core.hpp>

namespace bloom {
  IndexBuffer::IndexBuffer(const uint32_t* data, uint32_t count) : m_count(count) {
    GLCall(glad_glGenBuffers(1, &m_rendererID));
    GLCall(glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));
    GLCall(glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(data), data, GL_STATIC_DRAW));

    // Copy data to m_data
    m_data = new uint32_t[count];
    for (uint32_t i = 0; i < count; i++) {
      m_data[i] = data[i];
    }
  }

  IndexBuffer::~IndexBuffer() { GLCall(glad_glDeleteBuffers(1, &m_rendererID)); }

  void IndexBuffer::bind() const {
    GLCall(glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID));
  }

  void IndexBuffer::unbind() const { GLCall(glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); }
}  // namespace bloom
