#include <bloomCG/buffers/vertex_buffer_layout.hpp>
#include <bloomCG/core/core.hpp>

namespace bloom {

  template <typename T> VertexBufferLayout& VertexBufferLayout::push(uint32_t count) {
    ASSERT(false);
    return *this;
  }

  template <> VertexBufferLayout& VertexBufferLayout::push<float>(uint32_t count) {
    m_elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_stride += count * VertexBufferLayoutElement::getSizeOfType(GL_FLOAT);

    return *this;
  }

  template <> VertexBufferLayout& VertexBufferLayout::push<uint32_t>(uint32_t count) {
    m_elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_stride += count * VertexBufferLayoutElement::getSizeOfType(GL_UNSIGNED_INT);

    return *this;
  }

  template <> VertexBufferLayout& VertexBufferLayout::push<uint8_t>(uint32_t count) {
    m_elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_stride += count * VertexBufferLayoutElement::getSizeOfType(GL_UNSIGNED_BYTE);

    return *this;
  }
}  // namespace bloom
