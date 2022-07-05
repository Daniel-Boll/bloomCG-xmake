#pragma once

#include <bloomCG/core/common.hpp>

namespace bloom {

  class IndexBuffer {
  private:
    uint32_t m_rendererID;
    uint32_t m_count;
    uint32_t* m_data;

  public:
    IndexBuffer(const uint32_t* data, uint32_t count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    inline uint32_t getCount() const { return m_count; }
    inline uint32_t* getData() const { return m_data; }
  };

}  // namespace bloom
