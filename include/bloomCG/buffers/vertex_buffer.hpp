#pragma once

#include <bloomCG/core/common.hpp>

namespace bloom {

  class VertexBuffer {
  private:
    uint32_t m_rendererID;

  public:
    VertexBuffer(const void* data, uint32_t size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
  };

}  // namespace bloom
