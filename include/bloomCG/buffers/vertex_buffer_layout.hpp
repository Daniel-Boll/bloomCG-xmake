#pragma once

#include <bloomCG/core/common.hpp>

namespace bloom {
  struct VertexBufferLayoutElement {
    uint32_t type;
    uint32_t count;
    uint32_t normalized;

    static uint32_t getSizeOfType(uint32_t type) {
      switch (type) {
        case GL_FLOAT:
          return 4;
        case GL_UNSIGNED_INT:
          return 4;
        case GL_UNSIGNED_BYTE:
          return 1;
      }
      return 0;
    }
  };

  class VertexBufferLayout {
  private:
    std::vector<VertexBufferLayoutElement> m_elements;
    uint32_t m_stride;

  public:
    VertexBufferLayout() : m_stride(0) {}
    ~VertexBufferLayout() = default;

    template <typename T> VertexBufferLayout& push(uint32_t count);

    inline const std::vector<VertexBufferLayoutElement>& getElements() const { return m_elements; }
    inline const uint32_t getStride() const { return m_stride; }
  };
}  // namespace bloom
