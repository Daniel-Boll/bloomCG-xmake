#pragma once

#include <bloomCG/buffers/index_buffer.hpp>
#include <bloomCG/buffers/vertex_buffer.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/core/shader.hpp>

#include "bloomCG/buffers/vertex_array.hpp"
#include "bloomCG/buffers/vertex_buffer_layout.hpp"

namespace bloom {

  enum class CubeType {
    INDEXED,  // Create a cube using index buffer, which is cheaper and do not repeat vertices,
              // but cannot calculate normals accordingly
    REPEATED  // Create a cube using vertex array, which is more expensive and repeat vertices,
              // but has normals values correct
  };

  class Cube {
  protected:
    static bloom::IndexBuffer *m_indexBuffer;
    static bloom::VertexBufferLayout *m_vertexBufferIndexedLayout;
    static bloom::VertexBufferLayout *m_vertexBufferRepeatedLayout;

  protected:
    float m_size;
    glm::vec3 m_position;
    CubeType m_type;

    std::vector<float> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<unsigned int> m_indices;

    std::vector<float> m_vertexData;

    bloom::VertexArray *m_vertexArray;
    std::unique_ptr<bloom::VertexBuffer> m_vertexBuffer;

    void generateIndexedVertices();
    void generateRepeatedVertices();
    void generateNormals();

    void setupBuffers();
    void buildIndexedVertexData();
    void buildRepeatedVertexData();

    uint32_t *getIndices();

  public:
    Cube(float sideSize, glm::vec3 position, CubeType type = CubeType::INDEXED);

    void draw();
    void print();

    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);

    ~Cube();
  };
}  // namespace bloom
