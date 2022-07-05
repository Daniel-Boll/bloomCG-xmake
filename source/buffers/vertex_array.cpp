#include <bloomCG/buffers/vertex_array.hpp>

#include "bloomCG/buffers/vertex_buffer_layout.hpp"
#include "bloomCG/core/core.hpp"

namespace bloom {

  VertexArray::VertexArray() { GLCall(glad_glGenVertexArrays(1, &m_rendererID)); }
  VertexArray::~VertexArray() { GLCall(glad_glDeleteVertexArrays(1, &m_rendererID)); }

  void VertexArray::bind() const { GLCall(glad_glBindVertexArray(m_rendererID)); }

  void VertexArray::unbind() const { GLCall(glad_glBindVertexArray(0)); }

  void VertexArray::addBuffer(const VertexBuffer &buffer, const VertexBufferLayout &layout) {
    this->bind();
    buffer.bind();

    const auto &elements = layout.getElements();
    unsigned int offset = 0;

    for (uint32_t i = 0; i < elements.size(); i++) {
      const auto &element = elements[i];
      GLCall(glad_glEnableVertexAttribArray(i));

      /**
       * It can be used to indicate the structure of our data.
       * Example:
       * | Attribute 0 | Attribute 1 | Attribute 2 | Attribute 3 |
       * |  2 floats   |  3 floats   |  1 floats   |  1 uint     |
       *
       * Parameters: attributes id (coordinates) and in this case the only one (location = 0 on
       * shader)
       *              (count) number of attributes \
       *              (type) \
       *              (normalized?) \
       *              (stride) get to the next vertex \
       *              (offset) get to the next attribute \
       */
      GLCall(glad_glVertexAttribPointer(i, element.count, element.type, element.normalized,
                                        layout.getStride(), (const void *)(intptr_t)offset));

      offset += element.count * VertexBufferLayoutElement::getSizeOfType(element.type);
    }
  }
}  // namespace bloom
